/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include "table_skel.h"
#include "inet.h"
#include "message-private.h"
#include "network_server.h"
#include "sdmessage.pb-c.h"
#include <poll.h>
#include <fcntl.h>

int NFDESC = 4;
int TIMEOUT = 10;
int sockfd, connsockfd, nfds, kfds, i, j, check;
struct sockaddr_in server, client;
struct pollfd connections[4];
char str[1025];
int nbytes, compress_array = 0;
socklen_t size_client;

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port)
{
	//Cria socket TCP
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Erro ao criar socket\n");
		return -1;
	};

	//preenche estrutura server para bind
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//Faz bind
	if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Erro ao fazer bind\n");
		close(sockfd);
		return -1;
	};

	//Faz listen
	if (listen(sockfd, 0) < 0)
	{
		perror("Erro ao executar listen\n");
		close(sockfd);
		return -1;
	};

	return sockfd;
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket)
{
	size_client = sizeof(struct sockaddr);
	for (i = 0; i < NFDESC; i++)
	{
		connections[i].fd = -1;
	}

	connections[0].fd = listening_socket;
	connections[0].events = POLLIN;

	nfds = 1;

	//TODO REMOVER SOCKET DE CONNECTION em caso de erros
	//while ((connsockfd = accept(listening_socket, (struct sockaddr *)&client, &size_client)) != -1)
	while ((kfds = poll(connections, nfds, TIMEOUT)) >= 0)
	{
		if (kfds > 0)
		{
			if ((connections[0].revents & POLLIN) && (nfds < NFDESC)) //pedido na listening sockets
				if ((connections[nfds].fd = accept(connections[0].fd, (struct sockaddr *)&client, &size_client)) > 0)
				{									   //ligacao feita?
					connections[nfds].events = POLLIN; //esperar dados nesta socket
					nfds++;
				}
			for (i = 1; i < nfds; i++)
			{
				if (connections[i].revents & POLLIN)
				{
					//while (1)
					//{
					printf("A receber mensagem do cliente \n");

					struct message_t *receivedMsg;
					receivedMsg = network_receive(connections[i].fd);

					if (receivedMsg == NULL)
					{
						perror("Erro ao receber dados do cliente\n");
						close(connections[i].fd);
						connections[i].fd = -1;
						compress_array = 1;
						continue;
					}
					printf("A realizar invoke\n");
					if ((check = invoke(receivedMsg)) < 0)
					{
						perror("Table not inicialized\n");
						network_send(connections[i].fd, receivedMsg);
						continue;
					}
					if ((check = network_send(connections[i].fd, receivedMsg) < 0))
					{
						perror("Could not send message to client\n");
						continue;
					}
					//}
				}
				if ((connections[i].revents & POLLHUP) || (connections[i].revents & POLLERR))
				{
					close(connections[i].fd);
					connections[i].fd = -1;
					compress_array = 1;
				}
				if (compress_array)
				{
					compress_array = 0;
					for (i = 0; i < nfds; i++)
					{
						if (connections[i].fd == -1)
						{
							for (j = i; j < nfds; j++)
							{
								connections[j].fd = connections[j + 1].fd;
							}
							i--;
							nfds--;
						}
					}
				}
			}
		}
		//
	}
	return 0;
};

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket)
{

	struct message_t *msg = (struct message_t *)malloc(sizeof(struct message_t));

	MessageT *msg_received;
	uint8_t *buffer;
	char char_buffer[1024];
	int len = 0;
	int bytesRead;

	bytesRead = read(client_socket, (&len), sizeof(len));
	len = ntohl(len);
	if ((bytesRead = read_all(client_socket, char_buffer, len)) != len)
	{
		printf("Bytes lidos são diferentes, foram lidos %d e len é %d\n", bytesRead, len);
	}
	//printf("Bytes lidos são diferentes, foram lidos %d  e len é %d\n", bytesRead, len);
	buffer = malloc(len);
	memcpy(buffer, char_buffer, bytesRead);
	//printf("foram lidos %d \n", bytesRead);
	//buffer = (uint8_t *) &buffer_char;
	msg_received = message_t__unpack(NULL, len, buffer);
	if (msg_received == NULL)
	{
		fprintf(stdout, "error unpacking the message\n");
		return NULL;
	}

	printf("Depois do unpack, opcode =  %d\n", msg_received->opcode);
	switch (msg_received->opcode)
	{
	case MESSAGE_T__OPCODE__OP_SIZE:
		msg->opcode = 10;
		msg->c_type = 60;
		msg->datasize = 0;
		msg->data = NULL;
		break;
	case MESSAGE_T__OPCODE__OP_DEL:
		msg->opcode = 20;
		msg->c_type = 10;
		msg->key = strdup(msg_received->key);
		break;
	case MESSAGE_T__OPCODE__OP_GET:
		msg->opcode = 30;
		msg->c_type = 10;
		msg->datasize = msg_received->data_size;
		msg->data = malloc(msg->datasize);
		memcpy(msg->data, msg_received->data, msg->datasize);
		break;
	case MESSAGE_T__OPCODE__OP_PUT:
		msg->opcode = 40;
		msg->c_type = 30;
		msg->datasize = msg_received->data_size;
		msg->data = malloc(msg->datasize);
		memcpy(msg->data, msg_received->data, msg->datasize);
		msg->key = strdup(msg_received->key);
		break;
	case MESSAGE_T__OPCODE__OP_GETKEYS:
		msg->opcode = 50;
		msg->c_type = 60;
		msg->datasize = msg_received->data_size;
		msg->data = malloc(msg->datasize);
		memcpy(msg->data, msg_received->data, msg->datasize);
		break;
	case MESSAGE_T__OPCODE__OP_VERIFY:
		msg->opcode = OP_VERIFY;
		msg->c_type = CT_RESULT;
		msg->request_id = msg_received->request_id;
		break;
	default:
		msg->opcode = 99;
		msg->c_type = 60;
		msg->datasize = 0;
		msg->data = NULL;
	}
	message_t__free_unpacked(msg_received, NULL);
	return msg;
};

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg)
{
	MessageT msgToSend;
	message_t__init(&msgToSend);

	switch (msg->opcode)
	{
	case (OP_SIZE + 1):
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_SIZE_R;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
		msgToSend.data_size = msg->datasize;
		break;
	case (OP_DEL + 1):
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_DEL_R;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
		msgToSend.request_id = msg->request_id;
		break;
	case (OP_GET + 1):
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_GET_R;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_VALUE;
		msgToSend.data_size = msg->datasize;
		msgToSend.data = malloc(msg->datasize);
		memcpy(msgToSend.data, msg->data, msg->datasize);
		break;
	case (OP_PUT + 1):
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_PUT_R;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
		msgToSend.request_id = msg->request_id;
		break;
	case (OP_GETKEYS + 1):
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_GETKEYS_R;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_KEYS;
		msgToSend.data_size = msg->datasize;
		msgToSend.data = strdup(msg->data);
		break;
	case (OP_VERIFY + 1):
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_VERIFY_R;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
		msgToSend.request_id = msg->request_id;
		break;
	default:
		msgToSend.opcode = MESSAGE_T__OPCODE__OP_ERROR;
		msgToSend.c_type = MESSAGE_T__C_TYPE__CT_NONE;
	}
	printf("Msg vinda do invoke opcode = %d\n", msg->opcode);
	/*
	msgToSend.opcode = msg->opcode;
	msgToSend.c_type = msg->c_type;
	msgToSend.data_size = msg->datasize;
	memcpy(msgToSend.data, msg->data,msg->datasize);
	*/
	unsigned len = message_t__get_packed_size(&msgToSend);
	void *buffer = malloc(len);
	message_t__pack(&msgToSend, buffer);
	int len_n = htonl(len);
	//TO-DO libertar memoria
	if (write(client_socket, &len_n, sizeof(len_n)) != sizeof(len_n))
	{
		perror("Erro ao enviar tamanho da mensagem ao cliente\n");
		printf("sizeof buffer = %d", len);
		close(client_socket);
		return -1;
	}
	if ((nbytes = write_all(client_socket, buffer, len)) != len)
	{
		perror("Erro ao fazer write para o cliente. \n");
		close(client_socket);
		return -1;
	}
	free(buffer);
	return 0;
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close()
{
	close(sockfd);
	return 0;
}
