/**
 * @file couche2.h
 * @author Clément Payart, Louis Lapointe, Mael Schiemsky
 * @brief Header des fonctions pour la 2ème couche
 * @version 1
 * @date 2022-05-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef COUCHE2
#define COUCHE2

#include "../global.h"
#include "../couche1/couche1.h"

/**
 * @brief Ecrit le superblock à la position 0 sur le disque d0
 *
 * @param virtual_disk Structure disque contenant le superblock à écrire et l'accès au fichier d0
 */
void write_super_block(virtual_disk_t *virtual_disk);

/**
 * @brief Lit le superblock sur le disque d0
 *
 * @param virtual_disk Structure disque contenant le superblock à lire et l'accès au fichier d0
 */
void read_super_block(virtual_disk_t *virtual_disk);

/**
 * @brief Change la valeur du premier octet libre sur le superblock
 *
 * @param super_block superblock modifié
 * @param value valeur à inserer dans le champ first_free_byte de super_block
 */
void change_first_byte(super_block_t *super_block, unsigned int value);

/**
 * @brief Affichage du contenu du superblock
 *
 * @param superblock Superblock affiché
 */
void print_super_block(super_block_t superblock);

/**
 * @brief Lit la table d'inodes du fichier d0 contenu dans la structure virtual_disk_t
 *
 * @param virtual_disk structure contenant le fichier d0 et la table d'inodes
 */
void read_inodes_table(virtual_disk_t *virtual_disk);

/**
 * @brief Écrit sur la table d'inodes du fichier d0 contenu dans la structure virtual_disk_t
 *
 * @param virtual_disk structure contenant le fichier d0 et la table d'inodes
 */
void write_inodes_table(virtual_disk_t *virtual_disk);

/**
 * @brief Retourne l'indice de la première inode disponible sur la table d'inode
 *
 * @param virtual_disk disque contenant la table d'inodes
 * @return Indice de la table d'inode
 */
unsigned int get_unused_inode(virtual_disk_t *virtual_disk);

/**
 * @brief Initialise la table d'inodes : nom, taille  et position
 *
 * @param virtual_disk structure contenant l'inode à initialiser
 * @param filename nom du ficher
 * @param size taille du fichier
 * @param pos position du fichier
 */
void init_inode(virtual_disk_t *virtual_disk, const char *filename, unsigned int size, unsigned int pos);

/**
 * @brief Vide l'inode, valeurs à 0 et noms reinitialisés à ""
 *
 * @param inode inode remis à zéro
 */
void empty_inode(inode_t *inode);

/**
 * @brief Suppression de l'inode positionnée à l'indice indice
 *
 * @param virtual_disk structure contenant l'inode à supprimer
 * @param indice indice de l'inode à supprimer
 */
void delete_inode(virtual_disk_t *virtual_disk, unsigned int indice);

/**
 * @brief Fonction de test de la seconde couche
 *
 */
void cmd_dump_inode();

/**
 * @brief Initialise la table d'inode en vidant chacune des inodes
 *
 * @param virtual_disk structure contenant la table d'inode à vider
 */
void create_empty_inode_table(virtual_disk_t *virtual_disk);

#endif