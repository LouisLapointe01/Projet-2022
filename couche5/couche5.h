/**
 * @file couche5.h Header du fichier 5
 * @author Clément Payart, Louis Lapointe, Mael Schiemsky
 * @brief Header des fonctions pour la 5ème couche et l'installation du disque d0
 * @version 1
 * @date 2022-05-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef COUCHE5
#define COUCHE5

#include "../global.h"
#include "../couche1/couche1.h"
#include "../couche2/couche2.h"
#include "../couche3/couche3.h"
#include "../couche4/couche4.h"

/**
 * @brief Scinde la commande et argument et récupère les morceaux dans la structure cmd_t
 *
 * @param commande_et_arg Char contenant la commande et la/les arguments(s) potentiels
 * @param commande_char structure récupérant la commmande et la/les arguments(s) potentiels. Compte également le nombre d'argument
 */
void extraireCommande(char *commande_et_arg, cmd_t *commande_char);

/**
 * @brief Fonction 'ls' listant le contenu du catalogue
 *
 * @param virtual_disk disque contenant le catalogue de fichiers
 *
 */
void ls_disk(virtual_disk_t *virtual_disk);

/**
 * @brief Fonction 'ls -l' listant le contenu du catalogue de manière concise
 *
 * @param virtual_disk disque contenant le catalogue de fichiers
 *
 */
void ls_disk_court(virtual_disk_t *virtual_disk);

/**
 * @brief Scinde et interprète la commande et les potentiels arguments.
 *
 * @param commande_et_arg Commande et arguments écrit par l'utilisateur
 * @param virtual_disk disque du systeme, sera potentiellement modifié selon la commande appelée
 * @param session session de l'utilisateur
 * @return true L'utilisateur a utilisé la commande quit
 * @return false L'utilisateur a utilisé une autre commande que quit OU a fait une commande érronée
 */
bool interprete_commande(char *commande_et_arg, virtual_disk_t *virtual_disk, session_t session);

/**
 * @brief Installation du disque : creation du disque, de l'utilisateur root
 *        Ecrit sur le disque un fichier passwd contenant le login de root et le haché de son mot de passe
 *
 */
void installation_disque();

#endif