/**
 * @file couche1.h
 * @author Clément Payart, Louis Lapointe, Mael Schiemsky
 * @brief Header des fonctions pour la 1ère couche
 * @version 1
 * @date 2022-05-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef COUCHE1
#define COUCHE1

#include "../global.h"

/**
 * @brief Fonction fusionnant perror et exit
 *
 * @param Message Message à ecrire dans perror
 * @param code_erreur code d'erreur à écrire dans exit
 */
void error(const char *Message, int code_erreur);

/**
 * @brief Initialisation du disque : ouverture du fichier d0 et création des valeurs dans le superblock
 *
 * @param virtual_disk
 */
void init_disk_sos(virtual_disk_t *virtual_disk);

/**
 * @brief Calcule le nombre de block à créer selon la taille du fichier et la taille d'un block
 *
 * @param nb_byte taille du fichier
 * @param block_size taille du block
 * @return unsigned int nombre de block
 */
unsigned int compute_nblock(unsigned int nb_byte, int block_size);

/**
 * @brief Ecrit dans le fichier 'file' le block à la position pos
 *
 * @param block block contenant l'information à écrire
 * @param pos position (octet) à laquelle il faut écrire
 * @param file Fichier de destination
 */
void write_block(block_t block, unsigned int pos, FILE *file);

/**
 * @brief Lit dans le fichier 'file' le block à la position pos
 *
 * @param pos position (octet) à laquelle il faut lire
 * @param file Fichier de lecture
 * @return block_t block contenant l'information lue
 */
block_t read_block(unsigned int pos, FILE *file);

/**
 * @brief Affichage en hex sur le stdout du contenu du block
 *
 * @param block block lu
 */
void print_block(block_t block);

/**
 * @brief Concatenation du message par des espace jusqu'à ce que la taille du fichier atteigne un multiple de BLOCK_SIZE
 *
 * @param message Message potentiellement modifié
 */
void ajuster_message(unsigned char *message);

/**
 * @brief Ouvre le disque systeme d0 en lecture/écriture binaire
 *
 * @param virtual_disk disque
 */
void ouverture_disque(virtual_disk_t *virtual_disk);

/**
 * @brief Ferme le disque systeme d0
 *
 * @param virtual_disk
 */
void fermeture_disque(virtual_disk_t *virtual_disk);

/**
 * @brief Fonction test de la couche 1
 *
 */
void foo_test_couche1();

#endif