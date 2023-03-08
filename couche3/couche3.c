#include "couche3.h"

/* ========= HACHAGE ET VERIFICATION MDP ========= */

void hashageMdp(char *mdp, char *mdphash)
{
    char *item = malloc(65 * sizeof(char));
    strcpy(item, mdp);
    sha256ofString((BYTE *)item, mdphash);
}

bool login_exist(users_table_t *tablutilis, char *login, int *user_ID)
{
    for (int i = 0; i < NB_USERS; i++)
    {
        if (strcmp(tablutilis[i]->login, login) == 0)
        {
            *user_ID = i;
            return true;
        }
    }
    return false;
}

void new_passwd(users_table_t *tablutilis, int user_ID)
{
    assert(user_ID < NB_USERS);

    char new_passwd[STRLONG];
    char new_passwd_copy[STRLONG];

    do
    {
        printf("%s : Veuillez insérer votre nouveau mot de passe : \n", tablutilis[user_ID]->login);
        scanf("%s", new_passwd);
        printf("%s : Veuillez confirmer votre nouveau mot de passe : \n", tablutilis[user_ID]->login);
        scanf("%s", new_passwd_copy);
        if (strcmp(new_passwd, new_passwd_copy) != 0)
            fprintf(stderr, "Erreur : Les deux mots de passe ne sont pas identiques\n");
    } while (strcmp(new_passwd, new_passwd_copy) != 0);

    char new_passwd_hashed[SHA256_BLOCK_SIZE * 2 + 1];
    hashageMdp(new_passwd, new_passwd_hashed);

    strcpy(tablutilis[user_ID]->passwd, new_passwd_hashed);
    printf("Mot de passe enregistré.\n");
}

void add_user(virtual_disk_t *virtual_disk, char *login)
{
    bool is_new_user = true;
    int user_ID;
    // Si le login n'existe pas, c'est un nouvel utilisateur
    is_new_user = !login_exist(virtual_disk->users_table, login, &user_ID);
    // Verification que la table d'utilisateur n'est pas pleine
    if (!tab_user_is_full(virtual_disk->users_table))
    {
        if (is_new_user)
        {
            for (int i = 0; i < NB_USERS; i++)
            {
                if (strcmp(virtual_disk->users_table[i]->login, "") == 0)
                {
                    user_ID = i;
                    break;
                }
            }
            // Mise à jour de la table et création du passwd
            strcpy(virtual_disk->users_table[user_ID]->login, login);
            new_passwd(virtual_disk->users_table, user_ID);
            virtual_disk->super_block.number_of_users++;
            printf("Utilisateur %s crée.\n", login);
        }
    }
    else
        fprintf(stderr, "Erreur : la table utilisateur est pleine. Veuillez au préalable supprimer un utilisateur\n");
}

bool passwd_correct(users_table_t *tablutilis, char *passwd, int user_ID)
{
    char passwd_hash[SHA256_BLOCK_SIZE * 2 + 1];
    hashageMdp(passwd, passwd_hash);
    // Comparaison du login haché et du login haché inscrit dans la table
    if (strcmp(tablutilis[user_ID]->passwd, passwd_hash) == 0)
        return true;
    return false;
}

bool del_user_root_privilege(virtual_disk_t *virtual_disk, char *login)
{

    int user_ID;
    if (login_exist(virtual_disk->users_table, login, &user_ID))
    {
        strcpy(virtual_disk->users_table[user_ID]->login, "");
        strcpy(virtual_disk->users_table[user_ID]->passwd, "");

        printf("Utilisateur effacé avec succes\n");

        // Mise à jour du superblock
        virtual_disk->super_block.number_of_users--;
        virtual_disk->super_block.nb_blocks_used -= USER_SIZE;
        return true;
    }

    else
    {
        fprintf(stderr, "Erreur : login inexistant.\n");
        return false;
    }
}

// Jamais utilisée finalement, mais je laisse ici au cas où
bool change_passwd(users_table_t *tablutilis, char *login, char *mdp)
{

    int user_ID;
    if (!login_exist(tablutilis, login, &user_ID))
        return false;
    if (passwd_correct(tablutilis, mdp, user_ID))
        new_passwd(tablutilis, user_ID);
    else // Erreur de choix
        fprintf(stderr, "Mot de passe incorrect\n");

    return false;
}

void write_user_table(virtual_disk_t *virtual_disk)
{
    // On se place au début de la table d'utilisateur
    fseek(virtual_disk->storage, USER_START, SEEK_SET);
    // Ecriture sur le disque de la table d'utilisateur, 1 à 1
    for (int i = 0; i < NB_USERS; i++)
    {
        if (fwrite(&virtual_disk->users_table[i], sizeof(struct user_s), 1, virtual_disk->storage) != 1)
            error("Erreur ecriture fichier", ECRITURE_ERR);
    }
}

void read_user_table(virtual_disk_t *virtual_disk)
{
    fseek(virtual_disk->storage, USER_START, SEEK_SET);
    // Lecture sur le disque de la table d'utilisateur, 1 à 1
    for (int i = 0; i < NB_USERS; i++)
    {
        if (fread(&virtual_disk->users_table[i], sizeof(struct user_s), 1, virtual_disk->storage) != 1)
            error("Erreur ecriture fichier", ECRITURE_ERR);
    }
}

bool tab_user_is_full(users_table_t *tablutilis)
{
    for (int i = 1; i < NB_USERS; i++)
    {
        if (strcmp(tablutilis[i]->login, "") == 0)
            return false;
    }
    // Il n'y a pas de login "", la table est pleine
    return true;
}

bool tab_user_is_empty(users_table_t *tablutilis)
{
    // Compte nombre emplacement vide
    int nb_empty_slot = 0;
    for (int i = 0; i < NB_USERS; i++)
    {
        if (strcmp(tablutilis[i]->login, "") == 0)
            nb_empty_slot++;
    }
    // Si nb_empty_slot = NB_USER-1, la table est vide
    return (nb_empty_slot == NB_USERS);
}

bool login_in(users_table_t *users_table, session_t *session)
{
    char login[FILENAME_MAX_SIZE];
    char passwd[STRLONG];
    int index_user;
    int nb_tentative = 3;

    // Tentatives d'écriture du login illimitées
    do
    {
        printf("Veuillez saisir votre login.\n");
        scanf("%s", login);
        if (!login_exist(users_table, login, &index_user))
        {
            fprintf(stderr, "Erreur : ce login n'existe pas, veuillez réessayer\n");
        }
    } while (!login_exist(users_table, login, &index_user));

    // Trois tentatives pour écrire le mot de passe
    do
    {
        printf("Veuillez saisir votre mot de passe.\n");
        scanf("%s", passwd);
        if (!passwd_correct(users_table, passwd, index_user))
        {
            fprintf(stderr, "Mot de passe incorrect, veuillez réessayer\n");
            nb_tentative--;
        }

    } while (!passwd_correct(users_table, passwd, index_user) && (nb_tentative > 0));
    if (nb_tentative == 0)
        return false;

    session->userid = index_user;
    return true;
}

void init_users(users_table_t *tablutilis)
{
    for (int i = 0; i < NB_USERS; i++)
    {
        strcpy(tablutilis[i]->login, "");
        strcpy(tablutilis[i]->passwd, "");
    }
}

void foo_test_couche3()
{

    virtual_disk_t virtual_disk;
    init_disk_sos(&virtual_disk);

    init_users(virtual_disk.users_table);

    if (tab_user_is_empty(virtual_disk.users_table))
        printf("%s[OK]%s Table vide\n", GRN, NRM);
    else
        printf("%s[KO]%s Table vide\n", RED, NRM);

    add_user(&virtual_disk, "Clement");
    printf("%s\n", virtual_disk.users_table[0]->login);

    if (!tab_user_is_empty(virtual_disk.users_table))
        printf("%s[OK]%sTable non vide apres ajout user \n", GRN, NRM);
    else
        printf("%s[KO]%sTable non vide apres ajout user\n", RED, NRM);

    del_user_root_privilege(&virtual_disk, "Clement");

    if (tab_user_is_empty(virtual_disk.users_table))
        printf("%s[OK]%s Table vide après effacement\n", GRN, NRM);
    else
        printf("%s[KO]%s Table vide après effacement\n", RED, NRM);

    add_user(&virtual_disk, "Clement");
    add_user(&virtual_disk, "Louis");
    add_user(&virtual_disk, "Mael");
    add_user(&virtual_disk, "Rayan");
    add_user(&virtual_disk, "Rayan");
    add_user(&virtual_disk, "Dernier_user");

    if (tab_user_is_full(virtual_disk.users_table))
        printf("%s[OK]%s Table pleine\n", GRN, NRM);
    else
        printf("%s[KO]%s Table pleine\n", RED, NRM);

    write_user_table(&virtual_disk);
    del_user_root_privilege(&virtual_disk, "Clement");
    del_user_root_privilege(&virtual_disk, "Louis");
    del_user_root_privilege(&virtual_disk, "Mael");
    del_user_root_privilege(&virtual_disk, "Rayan");
    del_user_root_privilege(&virtual_disk, "Dernier_user");

    if (tab_user_is_empty(virtual_disk.users_table))
        printf("%s[OK]%s Table vide après reset\n", GRN, NRM);
    else
        printf("%s[KO]%s Table vide après reset\n", RED, NRM);

    read_user_table(&virtual_disk);

    if (tab_user_is_full(virtual_disk.users_table))
        printf("%s[OK]%s Table pleine après lecture\n", GRN, NRM);
    else
        printf("%s[KO]%s Table pleine après lecture\n", RED, NRM);
}
