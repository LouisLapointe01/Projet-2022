#include "couche1.h"

void error(const char *Message, int code_erreur)
{
    perror(Message);
    exit(code_erreur);
}

void init_disk_sos(virtual_disk_t *virtual_disk)
{
    struct stat st;
    // Creation du repertoire rep si il n'existe pas
    if (stat("./rep", &st) == -1)
        mkdir("./rep", S_IRWXU);
    system("./cmd_format rep 500000");

    // initialisation du superblock
    virtual_disk->storage = fopen("rep/d0", "rb+");
    virtual_disk->super_block.first_free_byte = FIRST_FREE_BYTE;
    virtual_disk->super_block.nb_blocks_used = (USER_SIZE) + (SUPER_BLOCK_SIZE);
    virtual_disk->super_block.number_of_files = 0;
    virtual_disk->super_block.number_of_users = 0;
}

unsigned int compute_nblock(unsigned int nb_byte, int block_size)
{
    unsigned int nb_block;
    if (nb_byte % block_size == 0)
        nb_block = nb_byte / block_size;
    else
        nb_block = nb_byte / block_size + 1;
    return nb_block;
}

void write_block(block_t block, unsigned int pos, FILE *file)
{
    fseek(file, pos, SEEK_SET);
    if (fwrite(block.data, BLOCK_SIZE, 1, file) != 1)
        error("Erreur ecriture fichier", ECRITURE_ERR);
}

block_t read_block(unsigned int pos, FILE *file)
{
    block_t block;
    fseek(file, pos, SEEK_SET);

    if (fread(block.data, BLOCK_SIZE, 1, file) != 1)
        error("Erreur lecture fichier", LECTURE_ERR);
    return block;
}

void print_block(block_t block)
{
    printf("Block : ");
    for (int i = 0; i < BLOCK_SIZE; i++)
        printf("%x", block.data[i]);
    printf("\n");
}

void ajuster_message(unsigned char *message)
{
    while ((strlen((char *)message)) % BLOCK_SIZE != 0)
        strcat((char *)message, " ");
}

void ouverture_disque(virtual_disk_t *virtual_disk)
{
    virtual_disk->storage = fopen("rep/d0", "rb+");
}

void fermeture_disque(virtual_disk_t *virtual_disk)
{
    fclose(virtual_disk->storage);
}

void foo_test_couche1()
{
    virtual_disk_t virtual_disk;
    init_disk_sos(&virtual_disk);
    block_t block;
    block_t reception;
    char message[4] = "TEST";
    memcpy(&block.data, &message, BLOCK_SIZE);
    write_block(block, 0, virtual_disk.storage);
    reception = read_block(0, virtual_disk.storage);

    int same_value = 0;
    for (int i = 0; i < BLOCK_SIZE; i++)
        if (block.data[i] == reception.data[i])
            same_value++;
    if (same_value == BLOCK_SIZE)
        printf("%s[OK]%s Lecture du block identique au block de départ\n", GRN, NRM);
    else
        printf("%s[KO]%s Lecture du block identique au block de départ\n", RED, NRM);
}
