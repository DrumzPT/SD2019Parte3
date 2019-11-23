/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "list-private.h"
//"list-private.h" ja faz import a list.h

/* Função que cria uma nova lista (estrutura list_t a ser definida pelo
 * grupo no ficheiro list-private.h).
 * Em caso de erro, retorna NULL.
 */
struct list_t *list_create()
{
	struct list_t *p = (struct list_t *)malloc(sizeof(struct list_t));

	if (p == NULL)
		return NULL;

	p->first = (struct node_t *)malloc(sizeof(struct node_t));
	p->first->entry = NULL;
	p->first->next = NULL;
	p->size = 0;

	return p;
}

/* Função que elimina uma lista, libertando *toda* a memoria utilizada
 * pela lista.
 */
void list_destroy(struct list_t *list)
{

	if (list != NULL)
	{
		struct node_t *current = (list->first);
		struct node_t *next;

		while ((current) != NULL)
		{
			next = current->next;
			entry_destroy(current->entry);
			free(current);
			current = next;
		}
		list->first = NULL;
		free(list);
	}
}
/* Função que adiciona no final da lista (tail) a entry passada como
 * argumento.
 * Retorna 0 (OK) ou -1 (erro).
 */
int list_add(struct list_t *list, struct entry_t *entry)
{

	if (list != NULL && entry != NULL)
	{
		struct node_t *curr_node = list->first;

		struct node_t *p = (struct node_t *)malloc(sizeof(struct node_t));
		p->entry = entry;
		p->next = NULL;

		if (list_size(list) == 0)
		{
			list->first = p;
			++list->size;
			return 0;
		}

		while (curr_node->next != NULL)
		{
			if (strcmp(curr_node->entry->key, entry->key) == 0)
			{
				free(p);
				entry_destroy(curr_node->entry);
				curr_node->entry = entry;
				return 0;
			}
			curr_node = curr_node->next;
		}

		curr_node->next = p;
		++list->size;

		return 0;
	}

	return -1;
}

/* Função que elimina da lista a entry com a chave key.
 * Retorna 0 (OK) ou -1 (erro).
 */
int list_remove(struct list_t *list, char *key)
{

	if (list != NULL && key != NULL && list_size(list) > 0)
	{
		struct node_t *node = list->first;
		struct node_t *nodeNext = NULL;

		if (strcmp((node->entry->key), key) == 0)
		{
			list->first = list->first->next;
			entry_destroy(node->entry);
			free(node);
			--list->size;
			return 0;
		}

		while ((node->next) != NULL)
		{
			if (strcmp((node->next->entry->key), key) == 0)
			{
				nodeNext = node->next;
				node->next = nodeNext->next;
				entry_destroy(nodeNext->entry);
				free(nodeNext);
				--list->size;
				return 0;
			}
			node = (node->next);
		}
	}
	return -1;
}

/* Função que obtém da lista a entry com a chave key.
 * Retorna a referência da entry na lista ou NULL em caso de erro.
 * Obs: as funções list_remove e list_destroy vão libertar a memória
 * ocupada pela entry ou lista, significando que é retornado NULL
 * quando é pretendido o acesso a uma entry inexistente.
*/
struct entry_t *list_get(struct list_t *list, char *key)
{

	if (list != NULL && key != NULL && list->first != NULL)
	{
		if (list->first->entry)
		{
			struct node_t *node = list->first;

			while (node != NULL)
			{
				if (strcmp((node->entry->key), key) == 0)
					return node->entry;
				node = (node->next);
			}
			return NULL;
		}
	}

	return NULL;
}

/* Função que retorna o tamanho (número de elementos (entries)) da lista,
 * ou -1 (erro).
 */
int list_size(struct list_t *list)
{

	if (list != NULL)
	{
		return list->size;
	}
	return -1;
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **list_get_keys(struct list_t *list)
{

	if (list != NULL && list_size(list) > 0)
	{
		char **p = calloc(list_size(list) + 1, sizeof(char *));
		struct node_t *node = list->first;
		int i = 0;

		while (node != NULL)
		{
			p[i] = strdup(node->entry->key);
			i++;
			node = (node->next);
		}
		p[i] = NULL;
		return p;
	}
	return NULL;
}

/* Função que liberta a memória ocupada pelo array das keys da tabela,
 * obtido pela função list_get_keys.
 */
void list_free_keys(char **keys)
{

	if (keys != NULL)
	{
		int i;
		for (i = 0; keys[i] != NULL; i++)
		{
			free(keys[i]);
		}
		free(keys);
	}
}

/**
*Função para imprimir a lista
*/
void list_print(struct list_t *list)
{
	if (list == NULL)
	{
		printf("A lista está a NULL");
	}
	else
	{
		struct node_t *node = list->first;

		while (node != NULL)
		{
			// printf("%s ->",(node->entry->value->data));
			node = (node->next);
		}

		printf(" NULL");
	}
}
