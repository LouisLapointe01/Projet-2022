#include "couche5.h"

void extraireCommande(char *commande_et_arg, cmd_t *commande_char)
{

    commande_char->tabArgs = malloc(sizeof(char *) * strlen(commande_et_arg));
    char delim[] = " ";
    // Séparation des commandes et arguments
    char *strToken = strtok(commande_et_arg, delim);
    commande_char->nbArgs = 0;

    // Tant qu'il y a des arguments à lire
    while (strToken != NULL)
    {
        if (commande_char->nbArgs == 0)
        {
            commande_char->tabArgs[0] = malloc(sizeof(strToken));
            strcpy(commande_char->tabArgs[0], strToken);
        }
        if (commande_char->nbArgs == 1)
        {
            commande_char->tabArgs[1] = malloc(sizeof(strToken));
            strcpy(commande_char->tabArgs[1], strToken);
        }

        if (commande_char->nbArgs == 2)
        {
            commande_char->tabArgs[2] = malloc(sizeof(strToken));
            strcpy(commande_char->tabArgs[2], strToken);
        }
        commande_char->nbArgs++;
        strToken = strtok(NULL, delim);
    }
}

void ls_disk(virtual_disk_t *virtual_disk)
{
    int compteur = 0;
    printf("\nFichier sur le disque : \n");
    // Parcours de la table d'inode pour lire fichier par fichier
    for (int i = 0; i < INODE_TABLE_SIZE; i++)
    {
        // On ne lit que les fichiers existant (pas les emplacements vide)
        if (strcmp(virtual_disk->inodes[i]->filename, ""))
        {
            printf("Nom du fichier : %s\n", virtual_disk->inodes[i]->filename);
            printf("Taille fichier : %d octets\n", virtual_disk->inodes[i]->size);
            printf("UID : %d\n", virtual_disk->inodes[i]->uid);
            // User right
            if (virtual_disk->inodes[i]->uright == rw)
                printf("Uright : rw\n");
            else if (virtual_disk->inodes[i]->uright == rW)
                printf("Uright : rW\n");

            else if (virtual_disk->inodes[i]->uright == Rw)
                printf("Uright : Rw\n");

            else if (virtual_disk->inodes[i]->uright == RW)
                printf("Uright : RW\n");

            // Other right
            if (virtual_disk->inodes[i]->oright == rw)
                printf("Oright : rw\n");

            else if (virtual_disk->inodes[i]->oright == rW)
                printf("Oright : rW\n");

            else if (virtual_disk->inodes[i]->oright == Rw)
                printf("Oright : Rw\n");

            else if (virtual_disk->inodes[i]->oright == RW)
                printf("Oright : RW\n");

            printf("Date de création : %s\n", virtual_disk->inodes[i]->ctimestamp);
            printf("Dernière modification : %s\n\n", virtual_disk->inodes[i]->mtimestamp);
            compteur++;
        }
    }
    if (compteur == 0)
        printf("Aucun fichier sur le disque\n");
}

void ls_disk_court(virtual_disk_t *virtual_disk)
{
    int compteur = 0;
    printf("\nFichier sur le disque : \n");
    // Parcours de la table d'inode pour lire fichier par fichier
    for (int i = 0; i < INODE_TABLE_SIZE; i++)
    {
        // On ne lit que les fichiers existant (pas les emplacements vide)
        if (strcmp(virtual_disk->inodes[i]->filename, ""))
        {
            printf("%s ", virtual_disk->inodes[i]->filename);
            printf("%d octets\n", virtual_disk->inodes[i]->size);
            compteur++;
        }
    }
    if (compteur == 0)
        printf("Aucun fichier sur le disque\n");
    printf("\n");
}

bool interprete_commande(char *commande_et_arg, virtual_disk_t *virtual_disk, session_t session)
{

    cmd_t commande_char;
    // Séparation de la commande et des potentiels arguments
    extraireCommande(commande_et_arg, &commande_char);

    // ls : listing du contenu du catalogue
    if (strcmp(commande_char.tabArgs[0], "ls") == 0)
    {

        if (commande_char.nbArgs == 2)
        {
            if (strcmp(commande_char.tabArgs[1], "-l\0") == 0)
                ls_disk_court(virtual_disk);
            else
                fprintf(stderr, "Erreur : Argument %s invalide\n", commande_char.tabArgs[1]);
        }
        else if (commande_char.nbArgs == 1)
            ls_disk(virtual_disk);
        else
            fprintf(stderr, "Erreur : Nombre d'argument incorrect\n");
    }

    // cat : affichage du contenu du fichier en paramètre
    else if (strcmp(commande_char.tabArgs[0], "cat") == 0)
    {
        int index_file = 0;
        if (commande_char.nbArgs == 2)
        {

            if (file_exist(commande_char.tabArgs[1], virtual_disk, &index_file))
            {
                /*Si l'utilisateur est root
                OU l'utilisateur est propriétaire de fichier et a les droits de lecture
                OU l'utilisateur n'est pas le propriétaire et a les droits de lecture */
                if ((session.userid == ROOT_UID) || ((session.userid == virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->uright == Rw) || (virtual_disk->inodes[index_file]->uright == RW))) || ((session.userid != virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->oright == Rw) || (virtual_disk->inodes[index_file]->oright == RW))))
                {
                    file_t file = {
                        .data = "",
                        .size = 0};

                    read_file(commande_char.tabArgs[1], &file, virtual_disk);
                    printf("%s\n", file.data);
                }
                else
                    fprintf(stderr, "Erreur : Vous n'avez pas les droits de lecture sur le fichier %s\n", commande_char.tabArgs[1]);
            }
            else
                fprintf(stderr, "Erreur : Le fichier <%s> n'existe pas\n", commande_char.tabArgs[1]);
        }
        else
            fprintf(stderr, "Erreur : Nombre d'argument incorrect\n");
    }

    // rm :efface le fichier en paramètre si la session est propriétaire/a les droits
    else if (strcmp(commande_char.tabArgs[0], "rm") == 0)
    {
        int index_file = 0;
        if (commande_char.nbArgs == 2)
        {
            if (file_exist(commande_char.tabArgs[1], virtual_disk, &index_file))
            {
                /*Si l'utilisateur est root
                OU l'utilisateur est propriétaire de fichier et a les droits d'écriture
                OU l'utilisateur n'est pas le propriétaire et a les droits d'écriture */
                if ((session.userid == ROOT_UID) || ((session.userid == virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->uright == rW) || (virtual_disk->inodes[index_file]->uright == RW))) || ((session.userid != virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->oright == rW) || (virtual_disk->inodes[index_file]->oright == RW))))
                {
                    delete_file(commande_char.tabArgs[1], virtual_disk);
                    // Ecriture sur le disque des modifications
                    write_inodes_table(virtual_disk);
                    write_super_block(virtual_disk);
                }
                else
                    fprintf(stderr, "Erreur : Vous n'avez pas les droits d'écriture sur le fichier %s\n", commande_char.tabArgs[1]);
            }
            else
                fprintf(stderr, "Erreur : Le fichier <%s> n'existe pas\n", commande_char.tabArgs[1]);
        }
    }

    // cr : creation du fichier en argument. Ce dernier  est de taille 0 au départ jusqu'à modification
    else if (strcmp(commande_char.tabArgs[0], "cr") == 0)
    {

        int index_file = 0;
        if (commande_char.nbArgs == 2)
        {
            if (virtual_disk->super_block.number_of_files < INODE_TABLE_SIZE)
            {
                if (!file_exist(commande_char.tabArgs[1], virtual_disk, &index_file))
                {
                    index_file = get_unused_inode(virtual_disk);
                    file_t file = {
                        .data = "",
                        .size = 0};
                    // Ecriture du fichier
                    write_file(commande_char.tabArgs[1], file, virtual_disk, session);
                    // Ecriture des modifications sur le disque
                    write_inodes_table(virtual_disk);
                    write_super_block(virtual_disk);
                    printf("Fichier %s crée\n", commande_char.tabArgs[1]);
                }
                else
                    fprintf(stderr, "Erreur : Le fichier %s existe déjà, impossible de le creer \n", commande_char.tabArgs[1]);
            }
            else
                fprintf(stderr, "Erreur : Capacité maximale de fichier atteinte\n");
        }
        else
            fprintf(stderr, "Erreur : Nombre d'argument incorrect\n");
    }

    // edit : modification d'un fichier déjà existant
    else if (strcmp(commande_char.tabArgs[0], "edit") == 0)
    {
        int index_file = 0;
        if (commande_char.nbArgs == 2)
        {
            if (file_exist(commande_char.tabArgs[1], virtual_disk, &index_file))
            {

                /*Si l'utilisateur est root
                OU l'utilisateur est propriétaire de fichier et a les droits d'écriture
                OU l'utilisateur n'est pas le propriétaire et a les droits d'écriture */
                if ((session.userid == ROOT_UID) || ((session.userid == virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->uright == rW) || (virtual_disk->inodes[index_file]->uright == RW))) || ((session.userid != virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->oright == rW) || (virtual_disk->inodes[index_file]->oright == RW))))
                {
                    file_t new_file;
                    printf("Modification du fichier %s : \n", commande_char.tabArgs[1]);
                    // Récupération du texte écrit par l'utilisateur
                    fgets((char *)new_file.data, sizeof(new_file.data), stdin);
                    // Suppression du \0 lors de la validation du texte
                    char *pos = strrchr((char *)new_file.data, '\n');
                    if (pos)
                        *pos = '\0';
                    write_file(commande_char.tabArgs[1], new_file, virtual_disk, session);
                    // Mise à jour du disque
                    write_inodes_table(virtual_disk);
                    write_super_block(virtual_disk);
                }
                else
                    fprintf(stderr, "Erreur : Vous n'avez pas les droits de modification\n");
            }
            else
                fprintf(stderr, "Erreur : Le fichier mis en paramètre n'existe pas \n");
        }
    }

    // Ecriture d'un fichier du systeme host sur le disque
    else if (strcmp(commande_char.tabArgs[0], "load") == 0)
    {
        if (commande_char.nbArgs == 2)
            load_file_from_host(commande_char.tabArgs[1], virtual_disk, session);
    }

    // Ecriture d'un fichier du disque sur le systeme host
    else if (strcmp(commande_char.tabArgs[0], "store") == 0)
    {
        if (commande_char.nbArgs == 2)
        {
            int index_file = 0;

            if (file_exist(commande_char.tabArgs[1], virtual_disk, &index_file))
            {
                /*Si l'utilisateur est root
                OU l'utilisateur est propriétaire de fichier et a les droits de lecture
                OU l'utilisateur n'est pas le propriétaire et a les droits de lecture */
                if ((session.userid == ROOT_UID) || ((session.userid == virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->uright == Rw) || (virtual_disk->inodes[index_file]->uright == RW))) || ((session.userid != virtual_disk->inodes[index_file]->uid) && ((virtual_disk->inodes[index_file]->oright == Rw) || (virtual_disk->inodes[index_file]->oright == RW))))
                    store_file_to_host(commande_char.tabArgs[1], virtual_disk);
                else
                    fprintf(stderr, "Erreur : Vous n'avez pas les droits de modification");
            }
            else
                fprintf(stderr, "Erreur : le fichier <%s> n'existe pas\n", commande_char.tabArgs[1]);
        }
    }

    // chown : changement de propriétaire du fichier, doit être fait par root ou par l'utilisateur lui même
    else if (strcmp(commande_char.tabArgs[0], "chown") == 0)
    {
        if (commande_char.nbArgs == 3)
        {
            int index_file = 0;
            int index_user;
            // Si le fichier existe
            if (file_exist(commande_char.tabArgs[1], virtual_disk, &index_file))
            {
                // Si l'utilisateur est root ou le propriétaire du fichier
                if (((session.userid == ROOT_UID) || session.userid == virtual_disk->inodes[index_file]->uid))
                {
                    // Verification que le login demandé existe
                    if (login_exist(virtual_disk->users_table, commande_char.tabArgs[2], &index_user))
                    {
                        virtual_disk->inodes[index_file]->uid = index_user;
                        write_inodes_table(virtual_disk);
                    }
                    else
                        printf("Utilisateur %s non existant\n", commande_char.tabArgs[2]);
                }
                else
                    fprintf(stderr, "Erreur : Vous n'avez pas les droits d'accès à ce fichier\n");
            }
            else
                fprintf(stderr, "Erreur : fichier <%s> non existant\n", commande_char.tabArgs[1]);
        }
        else
            fprintf(stderr, "Erreur : Argument(s) incorrect(s)\n");
    }

    // chmod : changement de droits du fichier en parametre pour tous les autres utilisateurs
    else if (strcmp(commande_char.tabArgs[0], "chmod") == 0)
    {

        // Verification des arguments : 3 arguments et le dernier doit être rw,rW,Rw ou RW
        if ((commande_char.nbArgs == 3) && (((strcmp(commande_char.tabArgs[2], "rw") == 0) || ((strcmp(commande_char.tabArgs[2], "rW") == 0) || ((strcmp(commande_char.tabArgs[2], "Rw") == 0) || ((strcmp(commande_char.tabArgs[2], "RW")) == 0))))))
        {
            int index_file = 0;
            // Si le fichier existe
            if (file_exist(commande_char.tabArgs[1], virtual_disk, &index_file))
            {
                // Si la session est root ou le propriétaire
                if ((session.userid == ROOT_UID) || session.userid == virtual_disk->inodes[index_file]->uid)
                {
                    if ((strcmp(commande_char.tabArgs[2], "rw") == 0))
                        virtual_disk->inodes[index_file]->oright = rw;
                    else if ((strcmp(commande_char.tabArgs[2], "rW") == 0))
                        virtual_disk->inodes[index_file]->oright = rW;
                    else if ((strcmp(commande_char.tabArgs[2], "Rw") == 0))
                        virtual_disk->inodes[index_file]->oright = Rw;
                    else if ((strcmp(commande_char.tabArgs[2], "RW") == 0))
                        virtual_disk->inodes[index_file]->oright = RW;
                    write_inodes_table(virtual_disk);
                }
            }
            else
                fprintf(stderr, "Erreur : fichier <%s> non existant\n", commande_char.tabArgs[1]);
        }
        else
            fprintf(stderr, "Erreur : Argument(s) incorrect(s)\n");
    }
    // listusers : liste les utilisateurs
    else if (strcmp(commande_char.tabArgs[0], "listusers") == 0)
    {
        for (int i = 0; i < NB_USERS; i++)
            printf("Utilisateur %d : %s\n", i, virtual_disk->users_table[i]->login);
    }

    // quit : quitte l'interpréteur de commande
    else if (strcmp(commande_char.tabArgs[0], "quit") == 0)
    {
        printf("Fermeture de l'interpréteur.\n");
        return true;
    }

    // adduser : commande réservée à root, ajoute un utilisateur. creation du login et du mot de passe
    else if (strcmp(commande_char.tabArgs[0], "adduser") == 0)
    {
        if (session.userid == 0)
        {
            char login[FILENAME_MAX_SIZE];
            printf("Login à creer?\n");
            scanf("%s", login);
            add_user(virtual_disk, login);
            write_user_table(virtual_disk);
            //(Permet d'empêcher à l'interpréteur de ne pas laisser le temps d'écrire la prochaine commande)
            fgetc(stdin);
        }
        else
            fprintf(stderr, "Erreur : Droit d'accès refusé\n");
    }

    // rmuser : commande réservée à root, supprime l'utilisateur en argument (sauf root)
    else if (strcmp(commande_char.tabArgs[0], "rmuser") == 0)
    {
        if (session.userid == 0)
        {
            int user_uid;
            if (commande_char.nbArgs == 2)
            {
                // Verification que le login existe et que ça n'est pas root
                if (login_exist(virtual_disk->users_table, commande_char.tabArgs[1], &user_uid) && strcmp(commande_char.tabArgs[1], "root") != 0)
                {
                    del_user_root_privilege(virtual_disk, commande_char.tabArgs[1]);
                    // Mise à jour du disque
                    write_user_table(virtual_disk);
                    write_super_block(virtual_disk);
                }

                else if (strcmp(commande_char.tabArgs[1], "root") == 0)
                    fprintf(stderr, "Erreur : Impossible d'effacer root\n");
                else
                    fprintf(stderr, "Erreur : Ce login n'existe pas\n");
            }
            else
                fprintf(stderr, "Erreur : Nombre d'argument erroné\n");
        }
        else
            fprintf(stderr, "Erreur : Droit d'accès refusé\n");
    }
    else if (strcmp(commande_char.tabArgs[0], "help") == 0)
    {
        printf("\nls [-l]                                          : liste le contenu du catalogue\n");
        printf("cat <nom de fichier>                             : affiche à l’écran le contenu d’un fichier si l’utilisateur a les droits\n");
        printf("rm <nom de fichier>                              : supprime un fichier du système si l’utilisateur a les droits\n");
        printf("cr <nom de fichier>                              : crée un nouveau fichier sur le système, le propriétaire est l’utilisateur\n");
        printf("edit <nom de fichier>                            : édite un fichier pour modifier son contenu si l’utilisateur a les droits\n");
        printf("load <nom de fichier>                            : copie le contenu d'un fichier du système 'hote' sur le système avec le même nom\n");
        printf("store <nom de fichier>                           : copie le contenu d’un fichier du système 'hote' si l'utilisateur a les droits\n");
        printf("chown <nom de fichier> <login autre utilisateur> : change le propriétaire d’un fichier si le demandeur a les droits\n");
        printf("chmod <nom de fichier> <droit>                   : change les droits d’un fichier pour tous les autres utilisateurs si le demandeur a les droits\n");
        printf("listusers                                        : liste les utilisateurs du système\n");
        printf("quit                                             : sort de l’interprète de commande et du programme en sauvegardant le système de fichiers sur le disque\n");

        if (session.userid == ROOT_UID)
        {
            printf("adduser                                          : ajouter un utilisateur\n");
            printf("rmuser <login>                                   : supprimer un utilisateur\n");
        }

        printf("\n");
    }

    else
        fprintf(stderr, "Erreur : Commande inconnue\n");

    return false;
}

void installation_disque()
{
    virtual_disk_t virtual_disk;
    // Initialisation du disque
    init_disk_sos(&virtual_disk);
    // Initialisation de la table d'utilisateurs
    init_users(virtual_disk.users_table);
    // Initialisation de la table d'inode
    create_empty_inode_table(&virtual_disk);
    // creation de root
    add_user(&virtual_disk, "root");

    // Contiendra root + passwd haché : 4+65+1
    file_t file_root_passwd = {
        .data = "root "};
    strcat((char *)file_root_passwd.data, virtual_disk.users_table[0]->passwd);

    session_t session_UID = {
        .userid = ROOT_UID};

    // Creation du fichier passwd sur d0
    write_file("passwd", file_root_passwd, &virtual_disk, session_UID);

    // Ecriture sur le disque des modifications
    write_super_block(&virtual_disk);
    write_inodes_table(&virtual_disk);
    write_user_table(&virtual_disk);

    fermeture_disque(&virtual_disk);
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "Nombre d'argument erroné\n");
        exit(ARG_ERR);
    }

    // Installation du disque
    if (strcmp(argv[1], "install") == 0)
    {
        // Si le fichier d0 n'existe pas, on démarre l'installation
        if (access("rep/d0", F_OK) != 0)
            installation_disque();
        else
            fprintf(stderr, "Erreur : Disque déjà installé sur votre pc\n");
    }

    // Fonction test pour la couche1
    else if (strcmp(argv[1], "couche1") == 0)
        foo_test_couche1();
    // Fonction test pour la couche2
    else if (strcmp(argv[1], "couche2") == 0)
        cmd_dump_inode();
    // Fonction test pour la couche3
    else if (strcmp(argv[1], "couche3") == 0)
        foo_test_couche3();
    // Fonction test pour la couche4
    else if (strcmp(argv[1], "couche4") == 0)
        foo_test_couche4();

    // Lancement de la connexion à l'OS et interprète de commande
    else if (strcmp(argv[1], "couche5") == 0)
    {
        virtual_disk_t virtual_disk;
        ouverture_disque(&virtual_disk);

        // Lecture du disque sur virtual_disk
        read_inodes_table(&virtual_disk);
        read_super_block(&virtual_disk);
        read_user_table(&virtual_disk);

        printf("\nBienvenue sur ScratchOS !\n\n");

        bool exit = false;
        bool is_connected;
        session_t session;

        // Renvoie false si la connexion échoue après trois tentatives
        is_connected = login_in(virtual_disk.users_table, &session);
        char commande[STRLONG];
        fgets(commande, sizeof(commande), stdin);
        if (is_connected)
        {
            // Tant que l'utilisateur ne fait pas 'quit'
            while (!exit)
            {
                printf("Veuillez saisir votre commande. help pour la liste des commandes\n");
                fgets(commande, sizeof(commande), stdin);
                commande[strcspn(commande, "\n")] = 0;
                exit = interprete_commande(commande, &virtual_disk, session);
            }
        }
        // fermeture du disque
        fermeture_disque(&virtual_disk);
    }
    else
        printf("Commande inconnue\n");
}
