#include "couche4.h"

char *timestamp()
{
    time_t current_time;
    char *c_time_string;

    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        (void)fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        (void)fprintf(stderr, "Failure to convert the current time.\n");
        exit(EXIT_FAILURE);
    }
    c_time_string[strlen(c_time_string) - 1] = '\0';
    return c_time_string;
}

bool file_exist(char *filename, virtual_disk_t *virtual_disk, int *index_file)
{
    for (int i = 0; i < INODE_TABLE_SIZE; i++)
    {
        if (strcmp(virtual_disk->inodes[i]->filename, filename) == 0)
        {
            *index_file = i;
            return true;
        }
    }
    return false;
}

void write_file(char *filename, file_t file, virtual_disk_t *virtual_disk, session_t session)
{
    bool is_new_file = true;
    int file_index_inode;

    // Verification si le fichier est déjà présent sur le disque
    is_new_file = !file_exist(filename, virtual_disk, &file_index_inode);

    // ajuster_message(file.data);
    file.size = strlen((char *)file.data);

    // Si il s'agit d'un nouveau fichier
    if (is_new_file)
    {

        // Récupération d'une nouvelle inode et de la position d'écriture sur le disque
        int new_inode_index = get_unused_inode(virtual_disk);
        unsigned int pos = virtual_disk->super_block.first_free_byte;

        // Initatialisation des informations de l'inode
        init_inode(virtual_disk, filename, file.size, pos);
        virtual_disk->inodes[new_inode_index]->nblock = compute_nblock(file.size, BLOCK_SIZE);
        virtual_disk->inodes[new_inode_index]->uid = session.userid;
        virtual_disk->inodes[new_inode_index]->uright = RW;
        virtual_disk->inodes[new_inode_index]->oright = rw;
        strcpy(virtual_disk->inodes[new_inode_index]->ctimestamp, timestamp());
        strcpy(virtual_disk->inodes[new_inode_index]->mtimestamp, timestamp());

        // Copie du fichier bloc par bloc
        for (int i = 0; i < virtual_disk->inodes[new_inode_index]->nblock; i++)
        {
            block_t block;
            memcpy(&block.data, &file.data[i * BLOCK_SIZE], BLOCK_SIZE);
            write_block(block, pos, virtual_disk->storage);
            pos += BLOCK_SIZE;
        }

        // Mise à jour de l'inode : premier octet libre pour écrire le prochain fichier, incrementation du nombre de fichier et ajustement du nombre de block écrit
        unsigned int new_free_byte = virtual_disk->super_block.first_free_byte + virtual_disk->inodes[new_inode_index]->size + 1;
        change_first_byte(&virtual_disk->super_block, new_free_byte);
        virtual_disk->super_block.number_of_files++;
        virtual_disk->super_block.nb_blocks_used += virtual_disk->inodes[new_inode_index]->nblock;
    }

    // Si le fichier existe déjà
    else
    {
        block_t block_zero = {
            .data = "    ",
        };
        block_t block;
        // calcul du nombre de block
        unsigned int nb_block = compute_nblock(file.size, BLOCK_SIZE);

        // Si le fichier est de taille supérieure au précédent
        if (file.size > virtual_disk->inodes[file_index_inode]->size)
        {

            // Suppression ancien fichier -> remplacement par blocks de 0
            unsigned int old_pos = virtual_disk->inodes[file_index_inode]->first_byte;
            for (int i = 0; i < virtual_disk->inodes[file_index_inode]->nblock; i++)
            {
                write_block(block_zero, old_pos, virtual_disk->storage);
                old_pos += BLOCK_SIZE;
            }
            // position du premier octet libre
            unsigned int new_pos = virtual_disk->super_block.first_free_byte;
            virtual_disk->inodes[file_index_inode]->first_byte = new_pos;

            // Ecriture du nouveau contenu à la fin de disque à la position new_pos
            for (int i = 0; i < nb_block; i++)
            {
                memcpy(&block.data, &file.data[i * BLOCK_SIZE], BLOCK_SIZE);
                write_block(block, new_pos, virtual_disk->storage);
                new_pos += BLOCK_SIZE;
            }

            // mise à jour super_block
            virtual_disk->super_block.nb_blocks_used = virtual_disk->super_block.nb_blocks_used + nb_block - virtual_disk->inodes[file_index_inode]->nblock; // taille fichier = taille fichier + taille_nv - taille_ancien
            unsigned int new_free_byte = virtual_disk->super_block.first_free_byte + file.size + 1;
            change_first_byte(&virtual_disk->super_block, new_free_byte);

            // mise à jour inode
            virtual_disk->inodes[file_index_inode]->nblock = nb_block;
            strcpy(virtual_disk->inodes[file_index_inode]->mtimestamp, timestamp());
            virtual_disk->inodes[file_index_inode]->size = file.size;
        }

        else
        {

            // Le fichier est de taille inférieure
            unsigned int actual_pos = virtual_disk->inodes[file_index_inode]->first_byte;
            // calcul du nombre de block
            unsigned int nb_block = compute_nblock(file.size, BLOCK_SIZE);
            unsigned int diff_size_block = virtual_disk->inodes[file_index_inode]->nblock - nb_block;

            // Ecriture du block à la position actual_pos
            for (unsigned int i = 0; i < nb_block; i++)
            {
                memcpy(&block.data, &file.data[i * BLOCK_SIZE], BLOCK_SIZE);
                write_block(block, actual_pos, virtual_disk->storage);
                actual_pos += BLOCK_SIZE;
            }

            // Effacement de la différence de fichier à l'emplacement actual_pos
            for (unsigned int i = 0; i < diff_size_block; i++)
            {
                write_block(block_zero, actual_pos, virtual_disk->storage);
                actual_pos += BLOCK_SIZE;
            }

            // mise à jour inode
            virtual_disk->inodes[file_index_inode]->nblock = nb_block;
            strcpy(virtual_disk->inodes[file_index_inode]->mtimestamp, timestamp());
            virtual_disk->inodes[file_index_inode]->size = file.size;

            // mise à jour super_block
            virtual_disk->super_block.nb_blocks_used = virtual_disk->super_block.nb_blocks_used - diff_size_block;
        }
    }
}

int read_file(char *filename, file_t *file, virtual_disk_t *virtual_disk)
{
    bool file_exist = false;
    unsigned int inode_indice_fichier;
    // Parcours de la table d'inode
    for (unsigned int i = 0; i < INODE_TABLE_SIZE; i++)
    {
        // Si le fichier existe déjà
        if (strcmp(virtual_disk->inodes[i]->filename, filename) == 0)
        {
            file_exist = true;
            inode_indice_fichier = i;
            break;
        }
    }

    // Si le fichier existe
    if (file_exist)
    {
        // Recupération des informations
        int nb_block = virtual_disk->inodes[inode_indice_fichier]->nblock;
        int pos_lecture = virtual_disk->inodes[inode_indice_fichier]->first_byte;

        block_t block_lecture;
        // Remise à zéro de file.data et .size
        strcpy((char *)file->data, "");
        file->size = 0;

        // Lecture block par block
        for (int i = 0; i < nb_block; i++)
        {
            block_lecture = read_block(pos_lecture, virtual_disk->storage);
            memcpy(&file->data[i * BLOCK_SIZE], &block_lecture, BLOCK_SIZE);
            pos_lecture += BLOCK_SIZE;
        }

        // calcul taille de file
        file->size = strlen((char *)file->data);

        return 1;
    }
    // Le fichier n'existe pas
    else
        return 0;
}

int delete_file(char *filename, virtual_disk_t *virtual_disk)
{
    bool file_does_exist = false;
    int file_index_inode;
    // Verification si le fichier existe déjà
    file_does_exist = file_exist(filename, virtual_disk, &file_index_inode);
    // Si le fichier existe
    if (file_does_exist)
    {
        // Remplacement du fichier par des blocs d'espace (trou)
        block_t block_espace = {
            .data = "    ",
        };
        // On se place au premier octet d'écriture du fichier existant
        int pos = virtual_disk->inodes[file_index_inode]->first_byte;

        for (int i = 0; i < virtual_disk->inodes[file_index_inode]->nblock; i++)
        {
            write_block(block_espace, pos, virtual_disk->storage);
            pos += BLOCK_SIZE;
        }
        // Mise à jour du superblock
        virtual_disk->super_block.nb_blocks_used -= virtual_disk->inodes[file_index_inode]->nblock;
        virtual_disk->super_block.number_of_files--;

        // Suppression de l'inode correspondante
        delete_inode(virtual_disk, file_index_inode);

        printf("Fichier <%s> effacé\n", filename);
        return 1;
    }

    return 0;
}

void load_file_from_host(char *host, virtual_disk_t *disk, session_t session)
{
    // ouverture du fichier host en lecture et vérification
    FILE *fdHost;
    fdHost = fopen(host, "rb");
    if (fdHost == NULL)
        error("Dans load_file_from_host : erreur à l'ouverture du fichier host", OUVERTURE_ERR);

    // création du file_t temporaire permetant la transition entre le fichier host et le fichier à créer
    file_t tmpFile;

    // récupération de la taille du fichier
    if (fseek(fdHost, 0, SEEK_END) != 0)
    {
        fclose(fdHost);
        error("Dans load_file_from_host : erreur fseek SEEK_END", POS_END);
    }
    tmpFile.size = ftell(fdHost);
    if (fseek(fdHost, 0, SEEK_SET) != 0)
    {
        fclose(fdHost);
        error("Dans load_file_from_host : erreur fseek SEEK_SET", POS_BIG);
    }

    // vérification taille puis copy les data du fichier dans le file_t
    if (tmpFile.size >= MAX_FILE_SIZE)
    {
        fclose(fdHost);
        error("Dans load_file_from_host : erreur fichier host trop volumineux", SIZE_FILE_ERR);
    }

    int tmp;
    for (int i = 0; i < tmpFile.size; i++)
    {
        tmp = fgetc(fdHost);
        memcpy(&tmpFile.data[i], &tmp, sizeof(int));
    }

    write_file(host, tmpFile, disk, session);

    file_t file_lecture = {
        .data = "",
        .size = 0};

    read_file(host, &file_lecture, disk);

    // fermeture du fichier host
    fclose(fdHost);
}

void store_file_to_host(char *fileName, virtual_disk_t *disk)
{
    // création d'un file_t
    file_t rdFile = {
        .size = 0};
    memcpy(&rdFile.data, "", 1);

    if (!read_file(fileName, &rdFile, disk))
        fprintf(stderr, "Erreur lecture\n");

    // création d'un fichier vierge sur l'host nommé fileName
    FILE *host;
    host = fopen(fileName, "wb");
    if (host == NULL)
        error("Dans store_file_to_host : erreur création fichier", CREATION_ERR);
    // copy des données du file_t dans le fichier vierge
    for (int i = 0; i < rdFile.size; i++)
        fputc(rdFile.data[i], host);
    // fermeture du fichier nouvellement créer
    fclose(host);
}

// WIP a refaire
void foo_test_couche4()
{

    virtual_disk_t virtual_disk;
    init_disk_sos(&virtual_disk);

    write_super_block(&virtual_disk);
    read_super_block(&virtual_disk);

    session_t session;
    session.userid = 1;
    file_t file1 = {
        .data = "Message test"};
    file1.size = strlen((char *)file1.data);

    file_t file_lecture;
    file_t file_reception;

    write_file("Fichier 1", file1, &virtual_disk, session);
    delete_file("Fichier 1", &virtual_disk);

    if (read_file("Fichier 1", &file_reception, &virtual_disk))
    {
        printf("OK lecture\n");
        printf("%s\n", file_reception.data);
    }

    write_file("Fichier 2", file1, &virtual_disk, session);
    write_file("Fichier 3", file1, &virtual_disk, session);
    write_file("Fichier 4", file1, &virtual_disk, session);
    write_file("Fichier 5", file1, &virtual_disk, session);
    delete_file("Fichier 5", &virtual_disk);
    write_file("Fichier 6", file1, &virtual_disk, session);
    write_file("Fichier 7", file1, &virtual_disk, session);
    write_file("Fichier 8", file1, &virtual_disk, session);
    write_file("Fichier 9", file1, &virtual_disk, session);
    write_file("Fichier 10", file1, &virtual_disk, session);
    write_file("Fichier 2", file1, &virtual_disk, session);

    load_file_from_host("fichier1.txt", &virtual_disk, session);

    if (read_file("Fichier test", &file_lecture, &virtual_disk))
        printf("fichier : %s\n", file_lecture.data);
}
