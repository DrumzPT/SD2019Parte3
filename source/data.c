/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include <stdlib.h>
#include <string.h>

#include "data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size
 */
struct data_t *data_create(int size){

    if(size <= 0)
        return NULL;

    struct data_t *p = (struct data_t *) malloc (sizeof(struct data_t));

    if(p == NULL)
        return NULL;

    p->datasize = size;
    p->data = malloc (size);

    if(p->data == NULL){
        free(p);
        return NULL;
    }
    return p;
}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data. não deve reservar memória para os dados e deve usar
 * diretamente o ponteiro que lhe é passado
 */
struct data_t *data_create2(int size, void * data){

    if (data == NULL || size <=0)
        return NULL;

    struct data_t *p = (struct data_t *) malloc (sizeof(struct data_t));

    if (p == NULL)
        return NULL;

    p->datasize = size;
    p->data = data;

    if (p->data == NULL){
        free(p);
        return NULL;
    }
    return p;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){

    if (data != NULL){
        free(data->data);
        free(data);
    }
}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 */
struct data_t *data_dup(struct data_t *data){
    if(data == NULL || data->datasize <= 0 || data->data == NULL )
	return NULL;

    struct data_t *p = data_create(data->datasize);
    memcpy(p->data,data->data,data->datasize);

    return p;

}
