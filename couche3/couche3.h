/**
 * @file couche3.h
 * @author Clément Payart, Louis Lapointe, Mael Schiemsky
 * @brief Header des fonctions pour la 3ème couche
 * @version 1
 * @date 2022-05-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef COUCHE3
#define COUCHE3

#include "../global.h"
#include "../couche1/couche1.h"
#include "../couche2/couche2.h"
#include "../Sha256/sha256.h"
#include "../Sha256/sha256_utils.h"

#define STRLONG 30

/**
 * @brief Initialise la table utilisateur avec un root
 *
 * @param tablutilis Table d'utilisateurs
 */
void init_users(users_table_t *tablutilis);

/**
 * @brief Ajoute un utilisateur
 *
 * @param virtual_disk Disque contenant la table d'utilisateur
 * @param login Login utilisateur
 */
void add_user(virtual_disk_t *virtual_disk, char *login);

/**
 * @brief Fonction réservée à root. Efface l'utilisateur; pas besoin de son mot de passe
 *
 * @param virtual_disk Disque contenant le tableau d'utilisateur. Peut potentiellement contenir le login à effacer
 * @param login login à effacer
 * @return true Le login a été effacé
 * @return false Erreur lors de la suppression du login
 */
bool del_user_root_privilege(virtual_disk_t *virtual_disk, char *login);

/**
 * @brief Verification que le login en paramètre est dans la table d'utilisateur
 *        Modification de l'indice user si le login existe
 *
 * @param tablutilis Table d'utilisateur contenant (ou non) le login
 * @param login login à vérifier
 * @param indice_user Indice du potentiel login, sera modifié si ce dernier existe
 * @return true Le login existe
 * @return false Le login n'existe pas
 */
bool login_exist(users_table_t *tablutilis, char *login, int *indice_user);

/**
 * @brief Vérifie si le tableau d'utilisateur est plein ou non
 *
 * @param tablutilis Tableau d'utilisateurs à vérifier
 * @return true Si le tableau est plein
 * @return false Si il reste de la place dans la bouche à clément
 */
bool tab_user_is_full(users_table_t *tablutilis);

/**
 * @brief Vérifie si le tableau d'utilisateur est vide ou non (on ne prends pas en compte l'utilisateur ROOT)
 *
 * @param tablutilis Table d'utilisateurs
 * @return true Si la table est vide
 * @return false Si la table n'est pas vide
 */
bool tab_user_is_empty(users_table_t *tablutilis);

/**
 * @brief Ecrit sur le disque la table d'utilisateur
 *
 * @param virtual_disk disque ou sera écrit la table d'utilisateur.
 */
void write_user_table(virtual_disk_t *virtual_disk);

/**
 * @brief Connection à la session. Demande d'inscrire son login et son mot de passe
 *
 * @param users_table Table d'utilisateur, sert de vérification au login et mot de passe
 * @param session si le login s'est bien déroulé, sauvegarde de l'index d'utilisateur dans la session
 *
 */
bool login_in(users_table_t *users_table, session_t *session);

/**
 * @brief Lit le disque d0 et récupère les informations dans la variable virtual disk
 *
 * @param virtual_disk disque contenant le tableau à lire
 */
void read_user_table(virtual_disk_t *virtual_disk);

/**
 * @brief Fonction de test des fonctions de la couche3
 *
 */
void foo_test_couche3();

#endif