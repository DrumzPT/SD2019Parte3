/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include <stdlib.h>
#include <string.h>

#include "entry.h"

/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados passados.
 */
struct entry_t *entry_create(char *key, struct data_t *data) {

    struct entry_t *p = (struct entry_t *) malloc (sizeof(struct entry_t));

    if(p == NULL || data == NULL)
        return NULL;

    p->key = key;
    p->value = data;

    return p;
}

/* função entry_initialize: removida!
 */

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry) {

    if (entry != NULL){
        data_destroy(entry->value);
        free(entry->key); //é necessário?
        free(entry);
    }
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry) {

    if(entry == NULL)
        return NULL;

    struct entry_t *p = (struct entry_t *) malloc (sizeof(struct entry_t));

    if (p == NULL)
        return NULL;

    p->key = strdup(entry->key);
    if(entry->key == NULL){
        free(p);
        return NULL;
    }

    p->value = data_dup(entry->value);
    if(entry->value == NULL){
        free(p->key);
        free(p);
        return NULL;
    }

    return p;
}
