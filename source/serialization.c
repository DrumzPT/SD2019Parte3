/*
 * Grupo 35
 * Gonçalo Duarte  nº46362
 * José Santos  nº51108
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

#include "data.h"
#include "entry.h"
#include "serialization.h"

/* Serializa uma estrutura data num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho do buffer
 * alocado ou -1 em caso de erro.
 */
int data_to_buffer(struct data_t *data, char **data_buf){
	if(data != NULL && data_buf != NULL){
		int buffsize = sizeof(int) + strlen(data->data) + 1;
		int sizeofdata;
		char * ptr;
		*data_buf = (char*) malloc(buffsize);
		ptr = *data_buf;
		char* dataVal = data->data;

		sizeofdata = htonl(data->datasize);
		memcpy(ptr, &sizeofdata, 4);
		ptr+=4;
		memcpy(ptr,dataVal, strlen(data->data) + 1);
		return buffsize;
	}
	return -1;
}

/* De-serializa a mensagem contida em data_buf, com tamanho
 * data_buf_size, colocando-a e retornando-a numa struct
 * data_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct data_t *buffer_to_data(char *data_buf, int data_buf_size){
	if(data_buf != NULL && data_buf_size > 0){
		char* sentData;
		int datasize;
		struct data_t *returnData;

		datasize= ntohl(*(int *) data_buf);
		data_buf+=4;
		sentData=(char*) malloc(datasize);
		//talvez datasize +1?
		memcpy(sentData,data_buf,datasize);
		if(datasize +4 != data_buf_size){
			printf("Não tem o mesmo tamanho, abortar");
			return NULL;
		}
		returnData = data_create2(datasize,sentData);
		/*
		strcpy(sentData,(const char*)data_buf);
		struct data_t *returnData = data_create2(data_buf_size,sentData);
		return returnData;
		*/
		return returnData;

	}
	return NULL;

}

/* Serializa uma estrutura entry num buffer que sera alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer alocado ou -1 em caso de erro.
 */
int entry_to_buffer(struct entry_t *data, char **entry_buf){

	if(data != NULL && entry_buf != NULL){
		int buffsize = sizeof(int) + strlen(data->key)  + sizeof(int) + (data->value->datasize);
		*entry_buf = malloc(buffsize);
		char* k= data->key;
		int klength = htonl(strlen(data->key )) ;
		int sizeofdata= htonl(data->value->datasize);
		char* val;
		char* ptr = *entry_buf;
		char* keyterminator = strdup("EOFKEY");

		//copia o tamanho da key para o inicio do buffer e avança 4 bytes
		memcpy(ptr,&klength,4);
		ptr+=4;
		//copia a key para o buffer e avança stren(data->key) bytes
		memcpy(ptr, k, strlen(data->key)  );
		ptr+=strlen(data->key) ;

		memcpy(ptr, keyterminator,strlen(keyterminator));
		ptr+=strlen(keyterminator);
		//
		memcpy(ptr, &sizeofdata, 4);
		ptr+=4;
		//
		memcpy(ptr, data->value->data, data->value->datasize);
		return buffsize;

	}
	return -1;


}

/* De-serializa a mensagem contida em entry_buf, com tamanho
 * entry_buf_size, colocando-a e retornando-a numa struct
 * entry_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size){
	if(entry_buf != NULL && entry_buf_size >0){
		int keylength;
		int sizeofdata;
		char *key ;
		char *dataString;
		char* keyterminator = strdup("EOFKEY");
		struct data_t *dataPointer;
		printf("entry_buf_size = %d\n", entry_buf_size );
		printf("strlen do buffer = %d \n ", strlen(entry_buf));
		keylength = ntohl(*(int *) entry_buf);
		entry_buf += 4;

		printf("Key length é : %d ----- \n", keylength );

		key = (char*) malloc(keylength );
		memcpy(key, entry_buf, keylength );
		entry_buf+= keylength + strlen(keyterminator);

		printf("Key é : %c ----- \n", key );

		sizeofdata = ntohl(*(int *) entry_buf);
		entry_buf +=4;
		dataString =  calloc(sizeofdata,sizeof(char));
		memcpy(dataString, entry_buf,sizeofdata);
		dataPointer = data_create2(sizeofdata,dataString);

		struct entry_t* new_entry = entry_create(key, dataPointer);

		return new_entry;
	}

	return NULL;


}
