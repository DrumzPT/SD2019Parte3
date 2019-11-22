/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include "message-private.h"
#include "inet.h"
#include <errno.h>
#include "sdmessage.pb-c.h"

void free_message(struct message_t *msg)
{
	if (msg->key)
		free(msg->key);
	if (msg->data)
		free(msg->data);
	free(msg);
}

int read_all(int socket, char *buf, int length)
{
	int bufsize = length;
	while (length > 0)
	{
		int result = read(socket, buf, length);
		if (result == 0)
		{
			return 0;
		}
		if (result < 0)
		{
			if (errno == EINTR)
				continue;
			perror("read failed:");
			return result;
		}
		buf += result;
		length -= result;
	}
	return bufsize;
};

int write_all(int sock, char *buf, int len)
{
	int bufsize = len;
	while (len > 0)
	{
		int res = write(sock, buf, len);
		if (res < 0)
		{
			if (errno == EINTR)
				continue;
			perror("write failed:");
			return res;
		}
		buf += res;
		len -= res;
	}
	return bufsize;
}
