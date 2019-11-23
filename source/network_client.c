/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */

#include "client_stub.h"
#include "sdmessage.pb-c.h"
#include "message-private.h"
#include "client_stub-private.h"
#include "inet.h"
#include <string.h>

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t *rtable)
{
	int sockfd;
	struct sockaddr_in server;
	//Cria socket TCP
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Erro ao criar socket TCP\n");
		return -1;
	}
	//Preenche estrutura server para estabelecer conexão
	server.sin_family = AF_INET;
	server.sin_port = htons(rtable->port);
	if (inet_pton(AF_INET, rtable->ip, &server.sin_addr) < 1)
	{
		printf("Erro ao converter IP\n");
		close(sockfd);
		return -1;
	}

	//Estabelece conexão com o servidor definido em server
	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Erro ao conectar-se ao servidor\n");
		close(sockfd);
		return -1;
	}

	rtable->socket = sockfd;

	return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct rtable_t *rtable, struct message_t *msg)
{
	int sockfd, size_to_read, nbytes;
	unsigned msg_buf_len;
	uint8_t *msg_buf;
	sockfd = rtable->socket;

	printf("No network send receive\n");

	//converte struct message_t para MessageT
	MessageT msgToSend;
	message_t__init(&msgToSend);
	switch (msg->opcode)
	{
	case OP_SIZE:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_SIZE;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;
		msgToSend.data_size = 0;
		msgToSend.data = NULL;
		break;
	case OP_DEL:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_DEL;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_KEY;
		msgToSend.key = strdup(msg->key);
		//malloc(strlen(msg->key));
		//memcpy(msgToSend.key, msg->key, strlen(msg->key));
		break;
	case OP_GET:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_GET;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_KEY;
		msgToSend.data_size = msg->datasize;
		msgToSend.data = malloc(msg->datasize);
		memcpy(msgToSend.data, msg->data, msg->datasize);
		break;
	case OP_PUT:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_PUT;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
		msgToSend.data_size = msg->datasize;
		msgToSend.data = malloc(msg->datasize);
		memcpy(msgToSend.data, msg->data, msg->datasize);
		msgToSend.key = malloc(strlen(msg->key));
		memcpy(msgToSend.key, msg->key, strlen(msg->key));
		break;
	case OP_GETKEYS:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;
		break;
	case OP_VERIFY:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_VERIFY;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
		msgToSend.opcode = msg->request_id;
		break;
	default:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_ERROR;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;
	}

	msg_buf_len = message_t__get_packed_size(&msgToSend);
	msg_buf = malloc(msg_buf_len);
	if (msg_buf == NULL)
	{
		fprintf(stdout, "malloc error\n");
		return NULL;
	}
	int msg_buf_size = msg_buf_len;
	message_t__pack(&msgToSend, msg_buf);
	//free msgToSend
	msg_buf_len = htonl(msg_buf_len);

	char *msg_buf_char = (char *)msg_buf;

	//write com o tamanho da mensagem
	if ((nbytes = write(sockfd, &msg_buf_len, sizeof(msg_buf_len))) != sizeof(msg_buf_len))
	{
		perror("Erro ao enviar tamanho da mensagem ao cliente\n");
		printf("sizeof msg_buf_len = %d\n", msg_buf_len);
		close(sockfd);
		return NULL;
	}

	printf("Enviados %d bytes, do tamanho da mensagem \n", nbytes);

	//Enviar a mensagem serializada para o server
	if ((nbytes = write_all(sockfd, msg_buf_char, msg_buf_size)) != msg_buf_size)
	{
		perror("Erro ao fazer write para o servidor. \n");
		close(sockfd);
		return NULL;
	}

	printf("Enviados %d bytes, da mensagem \n", nbytes);

	free(msg_buf);

	printf("\n----- À espera de resposta do servidor -----\n\n");
	//Receber mensagem de resposta

	if ((nbytes = read(sockfd, &size_to_read, 4)) != 4)
	{
		printf("bytes lidos:%u\n", nbytes);
		perror("Erro ao fazer read do tamanho da mensagem do servidor\n");
		close(sockfd);
		return NULL;
	}
	size_to_read = ntohl(size_to_read);
	printf("O tamanho da msg a ler é %d \n", size_to_read);
	uint8_t *msg_received_buff;
	msg_received_buff = malloc(size_to_read);

	read_all(sockfd, (void *)msg_received_buff, size_to_read);
	/*
  if((nbytes = read_all(sockfd,msg_received_buff,size_to_read)) < size_to_read){
    perror("Erro ao fazer read da mensagem do servidor");
    close(sockfd);
    return NULL;
  }
  */
	MessageT *msg_received = NULL;
	msg_received = message_t__unpack(NULL, size_to_read, msg_received_buff);
	if (msg_received == NULL)
	{
		printf("Erro ao dar unpack da resposta do cliente\n");
		return NULL;
	}

	struct message_t *msg_received_struct = (struct message_t *)malloc(sizeof(struct message_t));
	if (msg_received_struct == NULL)
	{
		printf("Erro ao gerar msg_received_struct\n");
		return NULL;
	}

	// Conversão de MessageT para struct message_t
	switch (msg_received->opcode)
	{
	case MESSAGE_T__OPCODE__OP_SIZE_R:
		msg_received_struct->opcode = OP_SIZE + 1;
		msg_received_struct->c_type = CT_RESULT;
		msg_received_struct->datasize = msg_received->data_size;
		break;
	case MESSAGE_T__OPCODE__OP_DEL_R:
		msg_received_struct->opcode = OP_DEL + 1;
		msg_received_struct->c_type = CT_NONE;
		msg_received_struct->request_id = msg_received->request_id;
		break;
	case MESSAGE_T__OPCODE__OP_GET_R:
		msg_received_struct->opcode = OP_GET + 1;
		msg_received_struct->c_type = CT_VALUE;
		msg_received_struct->datasize = msg_received->data_size;
		msg_received_struct->data = malloc(msg_received->data_size);
		memcpy(msg_received_struct->data, msg_received->data, msg_received->data_size);
		break;
	case MESSAGE_T__OPCODE__OP_PUT_R:
		msg_received_struct->opcode = OP_PUT + 1;
		msg_received_struct->c_type = CT_NONE;
		msg_received_struct->request_id = msg_received->request_id;
		break;
	case MESSAGE_T__OPCODE__OP_GETKEYS_R:
		msg_received_struct->opcode = OP_GETKEYS + 1;
		msg_received_struct->c_type = CT_KEYS;
		msg_received_struct->datasize = msg_received->data_size;
		msg_received_struct->data = malloc(msg_received->data_size);
		memcpy(msg_received_struct->data, msg_received->data, msg_received->data_size);
		break;
	default:
		printf("Erro ao traduzir msg do cliente\n");
	}

	free(msg_received_buff);
	message_t__free_unpacked(msg_received, NULL);
	return msg_received_struct;
}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtable_t *rtable)
{
	close(rtable->socket);
	return 0;
}
