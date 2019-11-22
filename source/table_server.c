/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include "network_server.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

void sigHandler()
{
	printf("Apahnei um sinal \n");
}

int main(int argc, char **argv)
{

	signal(SIGPIPE, sigHandler);

	if (argv[1] == NULL || argv[2] == NULL)
	{
		printf("Uso incorrecto, usar da seguinte forma: table-server <port> <n_lists> \n");
		printf("Exemplo de uso: ./table_server 54321 7 \n");
		return 0;
	}
	int server_socket;
	short port = (short)atoi(argv[1]);
	int n_lists = atoi(argv[2]);

	if (argc != 3)
		return -1;
	if ((server_socket = network_server_init(port)) == -1)
		return -1;
	if (n_lists < 1)
		return -1;
	if (table_skel_init(n_lists) == -1)
		return -1;
	printf("Servidor Inicializado com sucesso\n A escutar... \n");
	if (network_main_loop(server_socket) == -1)
		return -1;

	if (network_server_close() == -1)
		return -1;

	table_skel_destroy();
	return 0;
}
