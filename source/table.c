/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "table.h"
#include "table-private.h"

/* Função para criar/inicializar uma nova tabela hash, com n
 * linhas (n = módulo da função hash)
 * Em caso de erro retorna NULL.
 */
struct table_t *table_create(int n){

    if(n<=0)
        return NULL;

    struct table_t* p = (struct table_t*) malloc(sizeof(struct table_t));

    if (p == NULL)
        return NULL;

    p->size = 0;
    p->rows = n;
    p->table = (struct list_t**) malloc(sizeof(struct list_t*) * n);

    if (p->table == NULL) {
        free(p);
        return NULL;
    }

    int i;
    for(i = 0; i < n; i++) {
        p->table[i] = list_create();
    }

    return p;
}

/* Função para libertar toda a memória ocupada por uma tabela.
 */
void table_destroy(struct table_t *table){

    int n = table->rows;

    int i;
    for(i = 0; i < n; i++) {
        if(table->table[i] != NULL)
            list_destroy(table->table[i]);
    }
    free(table->table);
    free(table);
}

/* Função para adicionar um par chave-valor à tabela.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na tabela,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *table, char *key, struct data_t *value){
    if(table == NULL || key == NULL || value == NULL) {
		return -1;
    }
    int result = 1;

    char* k = strdup(key);
    struct data_t* v = data_dup(value);

    struct entry_t* new_entry = entry_create(k, v);

    int n = hash(k) % table->rows;
    struct list_t* l = table->table[n];

    if (list_get(l, k) != NULL){
      result = table_del(table,key);
    }
    result = list_add(l, new_entry);
    if(result == 0) {
        table->size = table->size + 1;
    }


    return result;

}

/* Função para obter da tabela o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou table_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da tabela,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função.
 * Devolve NULL em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key){
    if(table == NULL || key == NULL){
        return NULL;
    }

    char* k = strdup(key);
    if(k == NULL){
        return NULL;
    }

    int index = hash(k) % table->rows;

    struct entry_t *entrada = entry_dup(list_get(table->table[index],k));

    if(entrada == NULL){
        free(k);
        return NULL;
    }

    struct data_t *data = data_dup(entrada->value);

    if(data == NULL){
        free(k);
        entry_destroy(entrada);
        return NULL;
    }
    free(k);
    entry_destroy(entrada);
    return data;

}

/* Função para remover um elemento da tabela, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação table_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int table_del(struct table_t *table, char *key){
    if(table == NULL || key == NULL)
        return -1;

    char* k = key;
    if(k == NULL){
        return -1;
    }

    int index = hash(k) % table->rows;
    if(list_remove(table->table[index],k) == -1)
        return -1;

    table->size = table->size -1;

    return 0;

}

/* Função que devolve o número de elementos contidos na tabela.
 */
int table_size(struct table_t *table){
    return table->size;
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **table_get_keys(struct table_t *table){
    int i, j, index = 0, size;
    char **aux;

    if(table == NULL)
        return NULL;

    char** keyArray = calloc(table_size(table)+1,sizeof(char*));
    if(keyArray == NULL)
        return NULL;

    for(i = 0; i < table->rows; i++){
        struct list_t* l = table->table[i];
        aux = list_get_keys(l);
        size = list_size(l);

        for(j = 0; j < size; j++){
            if(aux[j] != NULL){
                keyArray[index] = strdup(aux[j]);
                index++;
            }
        }
    list_free_keys(aux);
    }
    //colocar o ultimo elemento a NULL
    keyArray[index] = NULL;
    return keyArray;

}

/* Função que liberta toda a memória alocada por table_get_keys().
 */
void table_free_keys(char **keys){
	if (keys != NULL) {

		int i = 0;

		while (keys[i] != NULL){
			free(keys[i]);
      i++;
		}
		free(keys);
	}
}

void table_print(struct table_t *table) {
    int i;
    if(table == NULL){
      printf("A lista está a NULL");
    } else {
      for(i = 1; table->table[i]; i++){
        list_print(table->table[i]);
      }
    }
}

/* Função para fazer hash à key
 */
int hash(char* key) {

    int n = strlen(key);
    int h = 0;
    int i;
    for(i = 0; i < n; i++) {
        h += key[i];
    }

    return h;
}
