#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRIVATE_H

struct task_t
{
	int op_n; // Operation ID
	int op;   // Operation type: 0 for del and 1 for put
	char *key;
	char *data;
	//struct message_t *message;
	int datasize; // Tamanho do bloco de dados
	struct task_t *next;
};
/*struct message_t
{
	short opcode; // código da operação na mensagem
	short c_type; // tipo do conteúdo da mensagem
	int request_id;
	char *key;
	struct data_t *data;
};

struct data_t
{
	int datasize; // Tamanho do bloco de dados
	void *data;   // Conteúdo arbitrário
};*/

void queue_init();
void queue_add_task(struct task_t *task);
struct task_t *queue_get_task();
struct task_t *create_task(struct message_t *msg);
void *process_task(void *params);
#endif
