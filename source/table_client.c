/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include "client_stub.h"
#include "data.h"
#include "entry.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

/*
  Interface para interagir com tabela remota.
  Uso: table-client <server_ip>:<server_port>
*/
void sigHandler()
{
	printf("Apahnei um sinal \n");
}

void printError()
{
	printf("Comando errado! \n Comandos devem vir da seguinte forma: \n");
	printf("put <key> <data> \n get <key> \n del <key> \n");
	printf("size \n getkeys \n quit \n");
}

int main(int argc, char **argv)
{
	struct rtable_t *server;
	char cmd[500];
	char *firstArg, *secondArg, *thirdArg;
	int result;
	struct data_t *data;
	struct entry_t *entry;

	//signal(SIGPIPE, sigHandler);

	if (argc != 2)
	{
		printf("Argumentos errados \n");
		printf("Uso: table-client <server_ip>:<server_port>\n");
		return -1;
	}

	server = rtable_connect(argv[1]);
	/*
  if(server ==  NULL){
    printf("Não foi possivel estabelecer ligação ao Servidor");
    return -1;
  }
*/
	while (1)
	{

		printf(">>");
		fgets(cmd, 500, stdin);

		if (cmd != NULL)
		{
			if (cmd[strlen(cmd) - 1] == '\n')
			{
				cmd[strlen(cmd) - 1] = '\0';
			}

			firstArg = strtok(cmd, " ");
		}
		else
		{
			printf("Necessario inserir comando \n");
			break;
		}

		if (strcmp(firstArg, "quit") == 0)
		{
			printf("Exiting \n");
			break;
		}
		else if (strcmp(firstArg, "put") == 0)
		{
			secondArg = strtok(NULL, " ");
			secondArg = strcat(secondArg, "\0");
			thirdArg = strtok(NULL, "\0");
			if (secondArg != NULL && thirdArg != NULL)
			{
				data = data_create(strlen(thirdArg) + 1);
				strcpy(data->data, thirdArg);
				entry = entry_create(strdup(secondArg), data);
				result = rtable_put(server, entry);

				if (result == 0)
					printf("PUT realizado com sucesso \n");
				else
				{
					printf("Erros ao realizar o PUT <%s> <%s> \n", secondArg, thirdArg);
				}
				//data_destroy(data);
				entry_destroy(entry);
			}
			else
			{
				printError();
			}
		}
		else if (strcmp(firstArg, "get") == 0)
		{
			secondArg = strtok(NULL, "\0");
			secondArg = strcat(secondArg, "\0");
			if (secondArg != NULL)
			{
				data = rtable_get(server, secondArg);
				if (data == NULL)
					printf("Não foi possivel obter valor da chave %s \n", secondArg);
				else
				{
					printf("O Valor da chave %s é:", secondArg);
					char *aux = (char *)data->data;
					printf(" %s \n", aux);
				}
				data_destroy(data);
				//free(secondArg);
			}
			else
				printError();
		}
		else if (strcmp(firstArg, "del") == 0)
		{
			secondArg = strtok(NULL, "\0");
			secondArg = strcat(secondArg, "\0");
			if (secondArg != NULL)
			{
				result = rtable_del(server, secondArg);
				if (result == 0)
					printf("Delete realizado com sucesso \n");
				else
					printf("Ocorreu um erro ao realizar o Delete \n");
				//free(secondArg);
			}
			else
				printError;
		}
		else if (strcmp(firstArg, "size") == 0)
		{
			result = rtable_size(server);
			printf("O tamanho da tabela é %d \n", result);
		}
		else if (strcmp(firstArg, "getKeys") == 0)
		{
			char **resultGK = rtable_get_keys(server);
			if (resultGK == NULL)
			{
				printf("Erro a obter chaves ou são inexistentes\n");
			}
			else
			{
				int i = 0;
				printf("Chaves: \n");
				while (resultGK[i] != NULL)
				{
					printf("Chave: %s", resultGK[i]);
					i++;
				}
				rtable_free_keys(resultGK);
			}
		}
		else
			printError();
	}
	if (rtable_disconnect(server) == 0)
		printf("Desconectado com sucesso \n");
	else
		printf("Erro a desconectar do servidor \n");

	return 0;
}
