#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "sdmessage.pb-c.h"

#define OP_SIZE 10
#define OP_DEL 20
#define OP_GET 30
#define OP_PUT 40
#define OP_GETKEYS 50
#define OP_VERIFY 60
#define OP_ERROR 99

#define CT_KEY 10
#define CT_VALUE 20
#define CT_ENTRY 30
#define CT_KEYS 40
#define CT_RESULT 50
#define CT_NONE 60

#define EOK "-EOK-"

struct message_t
{
	short opcode; /* código da operação na mensagem */
	short c_type; /* tipo do conteúdo da mensagem */
	int request_id;
	int datasize;
	char *data;
	char *key;
};

int read_all(int socket, char *buf, int length);

int write_all(int sock, char *buf, int len);

void free_message(struct message_t *msg);

#endif
