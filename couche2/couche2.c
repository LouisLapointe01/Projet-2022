#include "couche2.h"

void write_super_block(virtual_disk_t *virtual_disk)
{
    fseek(virtual_disk->storage, 0, SEEK_SET);
    if ((fwrite(&virtual_disk->super_block, sizeof(struct super_block_s), 1, virtual_disk->storage)) != 1)
    {
        error("Erreur ecriture superblock", ECRITURE_ERR);
    }
}

void read_super_block(virtual_disk_t *virtual_disk)
{
    fseek(virtual_disk->storage, 0, SEEK_SET);
    if (fread(&virtual_disk->super_block, sizeof(struct super_block_s), 1, virtual_disk->storage) != 1)
    {
        error("Erreur lecture superblock", LECTURE_ERR);
    }
}

void change_first_byte(super_block_t *super_block, unsigned int value)
{
    super_block->first_free_byte = value;
}

void print_super_block(super_block_t superblock)
{
    printf("%u ", superblock.first_free_byte);
    printf("%u ", superblock.nb_blocks_used);
    printf("%u ", superblock.number_of_files);
    printf("%u\n", superblock.number_of_users);
}

void read_inodes_table(virtual_disk_t *virtual_disk)
{
    fseek(virtual_disk->storage, INODES_START, SEEK_SET);
    for (int i = 0; i < INODE_TABLE_SIZE; i++)
    {
        if (fread(virtual_disk->inodes[i], sizeof(struct inode_s), 1, virtual_disk->storage) != 1)
            error("Erreur lecture inode_t", LECTURE_ERR);
    }
}

void write_inodes_table(virtual_disk_t *virtual_disk)
{
    fseek(virtual_disk->storage, INODES_START, SEEK_SET);
    for (int i = 0; i < INODE_TABLE_SIZE; i++)
    {
        if (fwrite(&virtual_disk->inodes[i], sizeof(struct inode_s), 1, virtual_disk->storage) != 1)
            error("Erreur ecriture fichier", ECRITURE_ERR);
    }
}

unsigned int get_unused_inode(virtual_disk_t *virtual_disk)
{
    for (unsigned int i = 0; i < INODE_TABLE_SIZE; i++)
    {
        if (strcmp(virtual_disk->inodes[i]->filename, "") == 0)
            return i;
    }
    return INODE_TABLE_SIZE;
}

void init_inode(virtual_disk_t *virtual_disk, const char *filename, unsigned int size, unsigned int pos)
{
    if (get_unused_inode(virtual_disk) != INODE_TABLE_SIZE)
    {
        unsigned int index_inode = get_unused_inode(virtual_disk);     // Recupération inode vide
        strcpy(virtual_disk->inodes[index_inode]->filename, filename); // Ecriture sur l'inode des informations du fichier
        virtual_disk->inodes[index_inode]->size = size;
        virtual_disk->inodes[index_inode]->first_byte = pos;
    }
    else
        printf("Table inode complete, veuillez supprimer un fichier d'abord\n");
}

void empty_inode(inode_t *inode)
{
    strcpy(inode->filename, "");
    inode->size = 0;
    inode->uid = 0;
    inode->uright = 0;
    inode->oright = 0;
    strcpy(inode->ctimestamp, "");
    strcpy(inode->mtimestamp, "");
    inode->nblock = 0;
    inode->first_byte = 0;
}

void delete_inode(virtual_disk_t *virtual_disk, unsigned int indice)
{
    for (unsigned int i = indice; i < INODE_TABLE_SIZE - 1; i++)
        memcpy(virtual_disk->inodes[i], virtual_disk->inodes[i + 1], sizeof(struct inode_s));
    empty_inode(virtual_disk->inodes[INODE_TABLE_SIZE - 1]);
    virtual_disk->super_block.nb_blocks_used -= INODE_SIZE;
}

void create_empty_inode_table(virtual_disk_t *virtual_disk)
{
    for (int i = 0; i < INODE_TABLE_SIZE; i++)
        empty_inode(virtual_disk->inodes[i]);
}

void cmd_dump_inode()
{
    virtual_disk_t virtual_disk;
    init_disk_sos(&virtual_disk);
    if (get_unused_inode(&virtual_disk) == 0)
        printf("%s[OK]%s Premiere inode disponible après initialisation\n", GRN, NRM);
    else
        printf("%s[KO]%s Premiere inode disponible après initialisation\n", RED, NRM);
    init_inode(&virtual_disk, "fichier_1", MAX_FILE_SIZE, FIRST_FREE_BYTE);
    if (get_unused_inode(&virtual_disk) == 1)
        printf("%s[OK]%s Deuxieme inode disponible après initialisation\n", GRN, NRM);
    else
        printf("%s[KO]%s Deuxieme inode disponible après initialisation\n", RED, NRM);

    init_inode(&virtual_disk, "fichier_2", MAX_FILE_SIZE, FIRST_FREE_BYTE + 1);
    // Suppression première inode
    delete_inode(&virtual_disk, 0);
    // Le fichier_2 doit désormais être en position 0 de la table d'inode
    if (strcmp(virtual_disk.inodes[0]->filename, "fichier_2") == 0)
        printf("%s[OK]%s Fichier_2 en premiere position apres avoir effacé la table\n", GRN, NRM);
    else
        printf("%s[KO]%s Fichier_2 en premiere position apres avoir effacé la table\n", RED, NRM);

    // Ecriture puis effacement de la table d'inode
    write_inodes_table(&virtual_disk);
    delete_inode(&virtual_disk, 0);

    // Lecture de la table d'inode depuis le disque, on doit pouvoir relire le fichier_2
    read_inodes_table(&virtual_disk);
    if (strcmp(virtual_disk.inodes[0]->filename, "fichier_2") == 0)
        printf("%s[OK]%s Fichier_2 en premiere position après lecture du disque\n", GRN, NRM);
    else
        printf("%s[KO]%s Fichier_2 en premiere position après lecture du disque\n", RED, NRM);
}