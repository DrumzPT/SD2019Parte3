#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"
#include "list-private.h"

struct table_t {
    int size;
    int rows;
    struct list_t** table;
};

/* Função para criar/inicializar uma nova tabela hash, com n linhas
 * (módulo da função HASH).
 */
struct table_t *table_create(int n);

void table_print(struct table_t *table);

/* Função para fazer hash à key
 */
int hash(char* key);

#endif
