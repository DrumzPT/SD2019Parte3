/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include "sdmessage.pb-c.h"
#include "table.h"
#include <netinet/in.h>
#include "serialization.h"
#include "message-private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <table_skel-private.h>
#include <pthread.h>

struct table_t *tabela;
pthread_mutex_t table_lock;
struct task_t *queue_head = NULL;
pthread_mutex_t queue_lock;
pthread_cond_t queue_not_empty;
pthread_t thread_id;
//id da ultima op -- count das op realizadas
int last_assigned, op_count;

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists)
{
	if ((tabela = table_create(n_lists)) == NULL)
		return -1;
	pthread_mutex_init(&table_lock, NULL);
	/* char *key = strdup("123abc");
	char *key2 = strdup("teste");
	char *key3 = strdup("outra");
	char *key4 = strdup("lmnop");
	struct data_t *value = data_create2(strlen("1234567abc") + 1, strdup("1234567abc"));
	struct data_t *value2 = data_create2(strlen("q1w23er45t") + 1, strdup("q1w23er45t"));
	table_put(tabela, key, value);
	table_put(tabela, key2, value);
	table_put(tabela, key3, value2);
	table_put(tabela, key4, value2); */
	printf("\n ------------------- Table Size Apos init  %d\n", table_size(tabela));
	queue_init();
	if (pthread_create(&thread_id, NULL, &process_task, NULL) != 0)
	{
		perror("\nThread não criada.\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}

void queue_init()
{
	pthread_mutex_init(&queue_lock, NULL);
	pthread_cond_init(&queue_not_empty, NULL);

} /*Função para inicializar queue_lock e queue_not_empty*/

void queue_add_task(struct task_t *task)
{
	pthread_mutex_lock(&queue_lock);
	if (queue_head == NULL)
	{ /* Adiciona na cabeça da fila */
		queue_head = task;
		task->next = NULL;
	}
	else
	{ /* Adiciona no fim da fila */
		struct task_t *tptr = queue_head;
		while (tptr->next != NULL)
			tptr = tptr->next;
		tptr->next = task;
		task->next = NULL;
	}
	pthread_cond_signal(&queue_not_empty); /* Desbloqueia a thread */
	pthread_mutex_unlock(&queue_lock);
}

struct task_t *queue_get_task()
{
	pthread_mutex_lock(&queue_lock);
	while (queue_head == NULL)
		pthread_cond_wait(&queue_not_empty, &queue_lock); /* BLOQUEIA A thread */
	struct task_t *task = queue_head;
	queue_head = task->next;
	pthread_mutex_unlock(&queue_lock);
	return task;
}

//pthread_cond_signal(&queue_not_empty); /* Desbloqueia a thread */
//exit = 1; // Pede a thread para terminar
void *process_task(void *exit)
{
	// TYpe cast de void para o tipo de params
	while (1 /*exit == 0*/)
	{
		struct task_t *task = queue_get_task(); // Sleep until there are tasks to process
		if (task->op == 1)
		{
			struct data_t *dataToPut = data_create2(task->datasize, task->data);
			pthread_mutex_lock(&table_lock);
			table_put(tabela, task->key, dataToPut);
			op_count++;
			pthread_mutex_unlock(&table_lock);
			printf("put realizado com sucesso \n");
		}
		else if (task->op == 0)
		{
			pthread_mutex_lock(&table_lock);
			table_del(tabela, task->key);
			op_count++;
			pthread_mutex_unlock(&table_lock);
			printf("del realizado com sucesso \n");
		}
	}
	return NULL;
}

/* Liberta toda a memória e recursos alocados pela função table_skel_init.
 */
void table_skel_destroy()
{
	table_destroy(tabela);
	//queue destroy
}

/* Executa uma operação na tabela (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, tabela nao incializada)
*/
int invoke(struct message_t *msg)
{
	if (msg == NULL)
	{
		printf("Mensagem não inicializada\n");
		return -1;
	}
	if (tabela == NULL)
	{
		printf("Tabela nao inicializada\n");
		return -1;
	}
	int size;
	struct data_t *getData;
	struct data_t *dataToPut;
	struct entry_t *entryReceived;
	char *key;

	char **keys;
	char *keyBuff;
	int i = 0;
	int buffsize = 0;
	int keysize;
	//OPPUT e OPGet precisam de serialização / descerializacao
	//Fazer enum de resposta
	//Enviar toda a info necessária através do data com o serialization
	//Neccesário descerializar a data
	//Necessario msg init?, feito fora
	// ou sdmessage__pack, feito fora
	switch (msg->opcode)
	{
	case OP_ERROR:
		printf("Erro a receber mensagem OP Code = OP_BAD\n");
		//return -1;
		break;
	case OP_SIZE:
		pthread_mutex_lock(&table_lock);
		size = table_size(tabela);
		pthread_mutex_unlock(&table_lock);
		msg->opcode = OP_SIZE + 1;
		msg->c_type = CT_RESULT;
		msg->datasize = size; //sera hton?
		break;
	case OP_DEL:
		key = (char *)malloc(msg->datasize);
		strcpy(key, msg->data);
		if (key == NULL)
		{
			printf("Erro ao ler key do buffer\n");
		}
		if (table_del(tabela, key) == 0)
		{
			msg->opcode = OP_DEL + 1;
			msg->c_type = CT_NONE;
		}
		else
		{
			msg->opcode = OP_ERROR;
			msg->c_type = CT_NONE;
			printf("Erro ao executar delete\n");
			//return -1;
		}
		break;
	case OP_GET:
		key = (char *)malloc(msg->datasize);
		strcpy(key, msg->data);
		pthread_mutex_lock(&table_lock);
		getData = table_get(tabela, key);
		pthread_mutex_unlock(&table_lock);
		if (getData != NULL)
		{
			msg->opcode = OP_GET + 1;
			msg->c_type = CT_VALUE;
			char *data_buf;
			//int buf_size = data_to_buffer(getData,&data_buf);
			msg->datasize = getData->datasize;
			//msg->data = getData->data;
			msg->data = malloc(getData->datasize);
			memcpy(msg->data, getData->data, getData->datasize);
			//será igual ou memcopy?
		}
		else
		{
			msg->opcode = OP_GET + 1;
			msg->c_type = CT_VALUE;
			msg->datasize = 0;
			msg->data = NULL;
			printf("Erro ao obter data\n");
			//return -1;
		}
		break;
	case OP_PUT:
		printf("key é = %s \n", msg->key);
		if ((dataToPut = data_create2(msg->datasize, msg->data)) == NULL)
		{
			printf("Erro ao criar data para inserir na tabela \n");
			return -1;
		}
		// Create task from message
		struct task_t *task = create_task(msg);
		// Add put request in the queue
		queue_add_task(task);
		// Respond
		//if (table_put(tabela, msg->key, dataToPut) == 0)
		msg->opcode = OP_PUT + 1;
		msg->c_type = CT_RESULT;
		msg->request_id = task->op_n;
		printf("Mensagem para o cliente preparada com sucesso \n");
		break;
		//falta o get keys, enviar todas as keys pela data, descontruir.las pelo \0
		//será que as keys podem vir a Null no caso de inexistencia
	case OP_GETKEYS:
		pthread_mutex_lock(&table_lock);
		keys = table_get_keys(tabela);
		pthread_mutex_unlock(&table_lock);
		if (keys != NULL)
		{
			msg->opcode = OP_GETKEYS + 1;
			msg->c_type = CT_KEYS;
			//Concatenar as keys todas para uma string
			keysize = strlen(keys[i]);
			buffsize += keysize;
			keyBuff = (char *)malloc(keysize);
			strcpy(keyBuff, keys[i]);
			i++;
			while (keys[i] != NULL)
			{
				keysize = strlen(keys[i]);
				buffsize += keysize;
				keyBuff = (char *)realloc(keyBuff, keysize);
				strcat(keyBuff, keys[i]);
				i++;
			}
			msg->datasize = buffsize;
			msg->data = malloc(buffsize);
			memcpy(msg->data, keyBuff, buffsize);
			//strcpy(msg->data, keyBuff);
			//msg->data = keyBuff; // ou memcpy?
			table_free_keys(keys);
			free(keyBuff); // se dermos free o msg->data fica a apontar para nada
		}

		break;
	default:
		printf("Nenhum tipo de mensagem detectado \n");
		return -1;
	}

	return 0;
}

struct task_t *create_task(struct message_t *msg)
{
	struct task_t *task = malloc(sizeof(struct task_t));
	if (task == NULL)
	{
		printf("Erro ao inicializar task\n ");
		return NULL;
	}
	task->op_n = last_assigned;
	last_assigned++;
	task->op = msg->opcode == OP_PUT ? 1 : 0;
	task->key = strdup(msg->key);

	task->data = msg->opcode == OP_PUT ? strdup(msg->data) : NULL;
	task->datasize = msg->opcode == OP_PUT ? msg->datasize : 0;
	task->next = NULL;
	return task;
}