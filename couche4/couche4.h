/**
 * @file couche4.h
 * @author Clément Payart, Louis Lapointe, Mael Schiemsky
 * @brief Header des fonctions pour la 4ème couche
 * @version 1
 * @date 2022-05-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef COUCHE4
#define COUCHE4

#include "../global.h"
#include "../couche1/couche1.h"
#include "../couche2/couche2.h"
#include "../couche3/couche3.h"

/**
 * @brief Retourne le temps actuel
 *
 * @return char* Temps
 */
char *timestamp();

/**
 * @brief Renvoie un booleen : true si le nom du fichier existe dans la table d'inode du disque, false sinon
 *
 * @param filename nom du fichier recherché dans le disque
 * @param virtual_disk disque contenant potentiellement le nom du fichier
 * @param index_file : Si le fichier existe, mémorise sa position dans la table d'inodes
 * @return true Le fichier est présent sur le disque
 * @return false Le fichier n'est pas présent sur le disque
 */
bool file_exist(char *filename, virtual_disk_t *virtual_disk, int *index_file);

/**
 * @brief Ecrit le contenu du fichier file dans le disque. Le nom du fichier sera filename.
 *        Les droits d'utilisateur et autre sont mis à 0, seul le propriétaire a le droit de lecture par défaut.
 *
 *
 * @param filename nom du fichier
 * @param file structure contenant contenu fichier et taille du fichier
 * @param virtual_disk disque sur lequel écrire
 * @param session session de l'utilisateur
 */
void write_file(char *filename, file_t file, virtual_disk_t *virtual_disk, session_t session);

/**
 * @brief Lecture du fichier sur le disque selon le nom. Stocke l'information dans file
 *
 * @param filename nom du fichier à lire
 * @param file contiendra le contenu du fichier
 * @param virtual_disk disque contenant potentiellement le fichier
 * @return int code de retour si la lecture s'est bien déroulée
 */
int read_file(char *filename, file_t *file, virtual_disk_t *virtual_disk);

/**
 * @brief Efface le fichier si celui ci existe. Remplace son contenu par des zéro
 *
 * @param filename Nom du fichier à effacer
 * @param virtual_disk disque contenant potentiellement filename
 * @return int
 */
int delete_file(char *filename, virtual_disk_t *virtual_disk);

/**
 * @brief Recupère le contenu d'un fichier extérieur au disque interne pour écrire sur ce dernier.
 *
 * @param host fichier contenant le texte à écrire sur le disque
 * @param disk disque sur lequel sera écrit le fichier
 * @param session session de l'utilisateur
 */
void load_file_from_host(char *host, virtual_disk_t *disk, session_t session);

/**
 * @brief Recupère le contenu d'un fichier du disque et l'écrit sur un fichier externe qui aura le même nom
 *
 * @param fileName Nom du fichier contenu (potentiellement) dans le fichier
 * @param disk disque contenant (?) le fichier filename
 */
void store_file_to_host(char *fileName, virtual_disk_t *disk);

/**
 * @brief Fonction de test des fonctions de la couche 4
 *
 */
void foo_test_couche4();

#endif