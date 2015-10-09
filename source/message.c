#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "data.h"
#include "entry.h"
#include "message.h"
#include "message-private.h"


int message_to_buffer(struct message_t *msg, char **msg_buf){
	if (msg == NULL)
		return -1;

	short opcode = msg->opcode;
	short c_type = msg->c_type;

	int size = SHORT_SIZE + SHORT_SIZE;
	int offset = size;

	/* Testar c_type da mensagem */
	if ( c_type == CT_RESULT ){
		size += INT_SIZE;
		*msg_buf = (char *) malloc( size );

		int result = htonl(msg->content.result);
		memcpy(*msg_buf + offset, &result, INT_SIZE);
	}

	else if( c_type == CT_VALUE){
		size += INT_SIZE;
		size += msg->content.data->datasize;

		*msg_buf = (char *) malloc(size);

		int datasize_htonl = htonl(msg->content.data->datasize);
		memcpy(*msg_buf + offset, &datasize_htonl, INT_SIZE);
		offset += INT_SIZE;

		memcpy(*msg_buf + offset, msg->content.data->data, msg->content.data->datasize);
	}

	else if( c_type == CT_KEY ){
		int keysize = strlen(msg->content.key);
		size += SHORT_SIZE;
		size += keysize;

		*msg_buf = (char *) malloc(size);

		int keysize_htons = htons(keysize);
		memcpy(*msg_buf + offset, &keysize_htons, SHORT_SIZE);

		offset += SHORT_SIZE;
		memcpy(*msg_buf + offset, msg->content.key, keysize);
	}

	else if( c_type == CT_KEYS ){
		size += INT_SIZE;
		int i;

		for (i = 0; msg->content.keys[i] != NULL ; ++i)
		{
			size += 4;
			size += strlen(msg->content.keys[i]);
		}

		*msg_buf = (char *) malloc(size);

		// numero de keys
		int nkeys = htonl(i);
		memcpy(*msg_buf + offset, &nkeys, INT_SIZE);
		offset += 4;

		for (i = 0; msg->content.keys[i] != NULL; ++i)
		{
			int presize = strlen(msg->content.keys[i]);
			int keysize = htonl(presize);
			memcpy(*msg_buf + offset, &keysize, SHORT_SIZE);
			offset += SHORT_SIZE;

			memcpy(*msg_buf + offset, msg->content.keys[i], presize);
			offset += presize;
		}
	}

	else if(c_type == CT_ENTRY){
		int keysize = strlen(msg->content.entry->key);

		//para keysize e key em si
		size += SHORT_SIZE;
		size += keysize;
		//para datasize e data em si
		size += INT_SIZE;
		size += msg->content.entry->value->datasize;

		*msg_buf = (char *) malloc(size);

		// keysize
		int keysize_htons = htons(keysize);
		memcpy(*msg_buf + offset, &keysize_htons, SHORT_SIZE);
		offset += SHORT_SIZE;

		// key em si
		memcpy(*msg_buf + offset, msg->content.entry->key, keysize);
		offset += strlen(msg->content.entry->key);

		int pre_datasize = msg->content.entry->value->datasize;
		int datasize = htonl(pre_datasize);
		memcpy(*msg_buf + offset, &datasize, 4);
		offset += 4;

		memcpy(*msg_buf + offset, msg->content.entry->value->data, pre_datasize);
	}
	// se c_type nao conhecido
	else
		size = -1;

	//opcode
	opcode = htons(opcode);
	memcpy( *msg_buf, &opcode, 2 );

	//c_type
	c_type = htons(c_type);
	memcpy(*msg_buf + SHORT_SIZE, &c_type, 2);

	return size;

}

struct message_t *buffer_to_message(char *msg_buf, int msg_size){

	short opcode, c_type;
	struct message_t *msg = (struct message_t *) malloc( sizeof( struct message_t ) );

	int offset = 0;

	//le opcode
	memcpy( &opcode, msg_buf, 2 );
	offset += 2;
	opcode = ntohs(opcode);

	//le c_type
	memcpy( &c_type, msg_buf + offset, 2 );
	offset += 2;
	c_type = ntohs(c_type);

	//atribuir valores a message_t
	msg->opcode = opcode;
	msg->c_type = c_type;

	// se eh result
	if( c_type == CT_RESULT )
	{
		//le result
		int result;
		memcpy(&result, msg_buf + offset, INT_SIZE);
		result = ntohl(result);

		msg->content.result = result;
		memcpy(&(msg->content.result), &result, INT_SIZE);
	}

	// se eh value
	else if( c_type == CT_VALUE ) {
		int datasize;
		memcpy(&datasize, msg_buf + offset, INT_SIZE);
		datasize = ntohl(datasize);

		offset += INT_SIZE;
		msg->content.data = (struct data_t *) malloc( sizeof(struct data_t) );
		msg->content.data->datasize = datasize;
		msg->content.data->data = (void *) malloc(datasize);
		memcpy(msg->content.data->data, msg_buf + offset, datasize);
	}

	// se eh key
	else if(c_type == CT_KEY){
		int keysize;
		memcpy(&keysize, msg_buf + offset, SHORT_SIZE);
		keysize = ntohs(keysize);

		offset += SHORT_SIZE;
		msg->content.key = (char *) malloc(keysize);
		memcpy(msg->content.key, msg_buf + offset, keysize);
	}

	// se eh keys
	else if ( c_type == CT_KEYS ){
		int num;
		memcpy( &num, msg_buf + offset, 4 );
		num = ntohl(num);

		offset += 4;

		msg->content.keys = (char **) malloc( sizeof(char *) * (num + 1) );

		int i;
		for( i = 0; i < num; i++ )
		{
			int size;
			memcpy( &size, msg_buf + offset, SHORT_SIZE );
			size = ntohs(size);

			offset += 4;

			memcpy( msg->content.keys[i], msg_buf + offset, size );
			offset += size;
		}

		msg->content.keys[i + 1] = NULL;
	}

	// se eh entry
	else if( c_type == CT_ENTRY )
	{
		// cria uma entry em union
		//msg->content.entry = (struct entry_t *) malloc(sizeof( struct entry_t ));

		// keysize
		int keysize;
		memcpy(&keysize, msg_buf + offset, SHORT_SIZE);
		keysize = ntohs(keysize);

		// key
		offset += SHORT_SIZE;
		char *key = (char*) malloc(keysize);
		memcpy(key, msg_buf + offset, keysize);

		// datasize
		int data_size;
		offset += keysize;
		memcpy(&data_size, msg_buf + offset, INT_SIZE);
		data_size = ntohl(data_size);

		// data
		offset += 4;
		void *entry_data = malloc(data_size);
		memcpy(entry_data, msg_buf + offset, data_size);

		// criar entry
		struct data_t *data = data_create2(data_size, entry_data);
		msg->content.entry = entry_create(key, data);
	}
	else{
		return NULL;
	}
	// se eh
	return msg;
}


void free_message(struct message_t *message) {
	int i;

	switch(message->c_type) {
		case CT_VALUE:
			data_destroy(message->content.data);
			break;
		case CT_ENTRY:
			entry_destroy(message->content.entry);
			break;
		case CT_KEY:
			free(message->content.key);
			break;
		case CT_KEYS:
			for (i = 0; message->content.keys[i] != NULL; i++)
				free(message->content.keys[i]);

			free(message->content.keys);
			break;
	}

	free(message);
}




