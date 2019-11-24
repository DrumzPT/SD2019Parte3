/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */

#include "data.h"
#include "entry.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "client_stub.h"
#include "client_stub-private.h"
#include "message-private.h"
#include "network_client.h"
#include "serialization.h"
#include <netinet/in.h>

/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtable_t *rtable_connect(const char *address_port)
{
	char *port, *address_port_cpy, *ip;
	struct rtable_t *rtable;
	char separador = ':';
	if (address_port == NULL)
	{
		printf("Address e Port recebidos a NULL \n");
		return NULL;
	}
	if ((rtable = (struct rtable_t *)malloc(sizeof(struct rtable_t))) == NULL)
	{
		printf("Erro ao alocar memoria para rtable");
		return NULL;
	}

	address_port_cpy = strdup(address_port);
	if ((ip = strtok((char *)address_port_cpy, &separador)) == NULL)
	{
		printf("Erro a ler ip do stdin");
		free(address_port_cpy);
		free(rtable);
		return NULL;
	}

	if ((port = strtok(NULL, &separador)) == NULL)
	{
		printf("Erro a obter porto");
		free(address_port_cpy);
		free(rtable);
		return NULL;
	}

	rtable->ip = strdup(ip);
	rtable->port = atoi(port);
	free(address_port_cpy);

	if (network_connect(rtable) != 0)
	{
		free(rtable);
		printf("Erro no network_connect");
		return NULL;
	}

	return rtable;
}

//TODO free message

/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtable_disconnect(struct rtable_t *rtable)
{
	if (rtable == NULL)
	{
		return -1;
	}
	int result;
	result = network_close(rtable);
	free(rtable);
	return result;
}

/* Função para adicionar um elemento na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry)
{
	printf("key: %s\ndata: %s \n", entry->key, entry->value->data);
	if (rtable == NULL || entry->key == NULL || entry->value == NULL)
	{
		return -1;
	}
	int id;
	struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
	if (message == NULL)
	{
		printf("Erro ao gerar message_t\n");
		return -1;
	}
	char *entrybuf;
	message->opcode = OP_PUT;
	message->c_type = CT_ENTRY;
	//int bufsize = entry_to_buffer(entry, &entrybuf);
	message->datasize = entry->value->datasize;
	message->data = entry->value->data;
	message->key = entry->key;

	struct message_t *message_returned = network_send_receive(rtable, message);
	free(message);
	if (message_returned == NULL)
	{
		//free(message_returned);
		return -1;
	}
	if (message_returned->opcode == (OP_PUT + 1))
	{
		//entry_destroy(entry);
		id = message_returned->request_id;
		//free(message_returned);
		return id;
	}
	else
	{
		free(message_returned);
		entry_destroy(entry);
		return -1;
	}
}

/* Função para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key)
{
	if (rtable == NULL || key == NULL)
		return NULL;

	struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
	if (message == NULL)
	{
		printf("Erro ao gerar message_t\n");
		return NULL;
	}
	message->opcode = OP_GET;
	message->c_type = CT_KEY;
	message->datasize = strlen(key);
	message->data = key;
	struct message_t *message_returned;

	if ((message_returned = network_send_receive(rtable, message)) == NULL)
	{
		free(message);
		return NULL;
	}
	free(message);
	struct data_t *data;
	if (message_returned->opcode == OP_GET + 1)
	{
		data = data_create2(message_returned->datasize, strdup(message_returned->data));
	}
	//data = buffer_to_data(message_returned->data, message_returned->datasize);
	else
	{
		data = (struct data_t *)malloc(sizeof(struct data_t));
		data->datasize = 0;
		data->data = NULL;
	}

	if (data == NULL)
	{
		return NULL;
	}
	//free(message->data);
	//free(message);
	free(message_returned);
	return data;
}

/* Função para remover um elemento da tabela. Vai libertar
 * toda a memoria alocada na respetiva operação rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtable_del(struct rtable_t *rtable, char *key)
{
	printf("key: %s\n", key);
	if (rtable == NULL || key == NULL)
		return -1;
	struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
	if (message == NULL)
	{
		printf("Erro ao gerar message_t\n");
		return -1;
	}
	int id;
	message->opcode = OP_DEL;
	message->c_type = CT_KEY;
	message->key = key;

	struct message_t *message_returned;
	if ((message_returned = network_send_receive(rtable, message)) == NULL)
	{
		free(message_returned);
		free(message);
		return -1;
	}

	//free(message);
	/* printf("OPCode = %d", message_returned->opcode);
	printf("(OP_DEL + 1) = %d", (OP_DEL + 1)); */
	if (message_returned->opcode == (OP_DEL + 1))
	{
		id = message_returned->request_id;
		free(message_returned);
		free(message);
		return id;
	}
	else
	{
		free(message_returned);
		free(message);
		return -1;
	}
}

/* Devolve o número de elementos contidos na tabela.
 */
int rtable_size(struct rtable_t *rtable)
{
	int size;
	if (rtable == NULL)
		return -1;
	struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
	if (message == NULL)
	{
		printf("Erro ao gerar message_t\n");
		return -1;
	}
	message->opcode = OP_SIZE;
	message->c_type = CT_NONE;
	struct message_t *message_returned;
	if ((message_returned = network_send_receive(rtable, message)) == NULL)
	{
		free(message);
		return -1;
	}
	free(message);
	size = message_returned->datasize;
	//free(message->data);
	//free(message);
	free(message_returned);
	return size;
}

/* Devolve um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable)
{
	if (rtable == NULL)
		return -1;
	struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
	if (message == NULL)
	{
		printf("Erro ao gerar message_t\n");
		return -1;
	}
	char *keysConcat = NULL;
	char **keyArray;
	char *token;

	message->opcode = OP_GETKEYS;
	message->c_type = CT_NONE;

	struct message_t *message_returned;
	if ((message_returned = network_send_receive(rtable, message)) == NULL)
	{
		free(message_returned);
		free(message);
		return keysConcat;
	}

	//free(message);
	/* printf("OPCode = %d", message_returned->opcode);
	printf("(OP_DEL + 1) = %d", (OP_DEL + 1)); */
	if (message_returned->opcode == (OP_GETKEYS + 1))
	{
		int i = 0;
		keysConcat = message_returned->data;
		keyArray = calloc(message_returned->datasize + 1, sizeof(char *));
		token = strtok(keysConcat, EOK);
		while (token != NULL)
		{
			keyArray[i] = strdup(token);
			token = strtok(NULL, EOK);
			i++;
		}
		keyArray[i] = NULL;
		free(message_returned);
		free(message);

		return keyArray;
	}
	else
	{
		free(message_returned);
		free(message);
		return keysConcat;
	}
}

/* Verifica se a operação identificada por op_n foi executada.
*	0 False, 1 True, -1 op_n inexistente, -2 erro
*/
int rtable_verify(struct rtable_t *rtable, int op_n)
{
	printf("OP nr: %d \n ", op_n);
	if (rtable == NULL)
		return -2;
	struct message_t *message = (struct message_t *)malloc(sizeof(struct message_t));
	if (message == NULL)
	{
		printf("Erro ao gerar message_t\n");
		return -2;
	}
	message->opcode = OP_VERIFY;
	message->c_type = CT_RESULT;
	message->request_id = op_n;
	struct message_t *message_returned;
	if ((message_returned = network_send_receive(rtable, message)) == NULL)
	{
		free(message);
		return -2;
	}
	free(message);
	return message_returned->request_id;
}

/* Liberta a memória alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys)
{
}
