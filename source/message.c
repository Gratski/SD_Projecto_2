#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "data.h"
#include "entry.h"
#include "message-private.h"

int message_to_buffer(struct message_t *msg, char **msg_buf){
	if (msg == NULL || validate_msg(msg) < 0)
		return -1;

	//obter opcode
	short opcode = msg->opcode;
	//obter c_type
	short c_type = msg->c_type;

	int size = SHORT_SIZE + SHORT_SIZE;
	int offset = size;

	//CT_RESULT
	if ( c_type == CT_RESULT ){
		size += INT_SIZE;
		*msg_buf = (char *) malloc(size);

		if ( msg_buf == NULL )
			return -1;

		int result = htonl(msg->content.result);
		memcpy(*msg_buf + offset, &result, INT_SIZE);
	}
	//CT_VALUE
	else if( c_type == CT_VALUE ){
		size += INT_SIZE;
		size += msg->content.data->datasize;

		*msg_buf = (char *) malloc(size);

		if (msg_buf == NULL)
			return -1;

		int datasize_htonl = htonl(msg->content.data->datasize);

		memcpy(*msg_buf + offset, &datasize_htonl, INT_SIZE);
		offset += INT_SIZE;

		memcpy(*msg_buf + offset, msg->content.data->data, msg->content.data->datasize);
	}
	//CT_KEY
	else if( c_type == CT_KEY ){
		int keysize = strlen(msg->content.key);
		size += SHORT_SIZE;
		size += keysize;

		*msg_buf = (char *) malloc(size);

		if ( msg_buf == NULL )
			return -1;

		int keysize_htons = htons(keysize);
		memcpy(*msg_buf + offset, &keysize_htons, SHORT_SIZE);

		offset += SHORT_SIZE;
		memcpy(*msg_buf + offset, msg->content.key, keysize);
	}
	//CT_KEYS
	else if(c_type == CT_KEYS){
		// num_keys
		size += INT_SIZE;
		int i;

		for (i = 0; msg->content.keys[i] != NULL ; i++)
		{
			// key_size
			size += SHORT_SIZE;
			// key length
			size += strlen(msg->content.keys[i]);
		}

		*msg_buf = (char *) malloc(size);

		if (msg_buf == NULL)
			return -1;

		// numero de keys
		int nkeys = htonl(i);
		memcpy( *msg_buf + offset, &nkeys, INT_SIZE );
		offset += INT_SIZE;

		for ( i = 0; msg->content.keys[i] != NULL; i++ )
		{
			int presize = strlen( msg->content.keys[i] );
			int keysize = htons( presize );
			memcpy( *msg_buf + offset, &keysize, SHORT_SIZE );
			offset += SHORT_SIZE;

			memcpy( *msg_buf + offset, msg->content.keys[i], presize );
			offset += presize;
		}
	}
	//CT_ENTRY
	else if(c_type == CT_ENTRY){
		int keysize = strlen(msg->content.entry->key);

		//para keysize e key em si
		size += SHORT_SIZE;
		size += keysize;
		//para datasize e data em si
		size += INT_SIZE;
		size += msg->content.entry->value->datasize;

		*msg_buf = (char *) malloc(size);

		if (msg_buf == NULL)
			return -1;

		// keysize
		int keysize_htons = htons(keysize);
		memcpy(*msg_buf + offset, &keysize_htons, SHORT_SIZE);

		// key em si
		offset += SHORT_SIZE;
		memcpy(*msg_buf + offset, msg->content.entry->key, keysize);

		int pre_datasize = msg->content.entry->value->datasize;
		int datasize = htonl(pre_datasize);

		offset += strlen(msg->content.entry->key);
		memcpy(*msg_buf + offset, &datasize, INT_SIZE);

		offset += INT_SIZE;
		memcpy(*msg_buf + offset, msg->content.entry->value->data, pre_datasize);
	}

	//OP_CODE
	opcode = htons(opcode);
	memcpy(*msg_buf, &opcode, SHORT_SIZE);

	//C_TYPE
	c_type = htons(c_type);
	memcpy(*msg_buf + SHORT_SIZE, &c_type, SHORT_SIZE);

	return size;
}


struct message_t *buffer_to_message(char *msg_buf, int msg_size){
	if (msg_buf == NULL || msg_size < 0)
		return NULL;

	short opcode, c_type;
	struct message_t *msg = (struct message_t *) malloc(sizeof(struct message_t));

	if (msg == NULL)
		return NULL;

	int offset = 0, buffer_size;

	//ler opcode
	memcpy(&opcode, msg_buf, SHORT_SIZE);
	offset += SHORT_SIZE;
	opcode = ntohs(opcode);

	if (validate_opcode(opcode) < 0) {
		free(msg);
		return NULL;
	}

	//ler c_type
	memcpy(&c_type, msg_buf + offset, SHORT_SIZE);
	offset += SHORT_SIZE;
	c_type = ntohs(c_type);

	msg->opcode = opcode;
	msg->c_type = c_type;

	//CT_RESULT
	if(c_type == CT_RESULT)
	{
		//ler result
		int result;
		memcpy(&result, msg_buf + offset, INT_SIZE);
		result = ntohl(result);
		msg->content.result = result;

		buffer_size = offset + INT_SIZE;
	}
	//CT_VALUE
	else if( c_type == CT_VALUE ) {
		int datasize;
		memcpy(&datasize, msg_buf + offset, INT_SIZE);
		datasize = ntohl(datasize);

		offset += INT_SIZE;
		msg->content.data = (struct data_t *) malloc(sizeof(struct data_t));

		if (msg->content.data == NULL) {
			free_message(msg);
			return NULL;
		}

		msg->content.data->datasize = datasize;
		msg->content.data->data = (void *) malloc(datasize);

		if (msg->content.data->data == NULL) {
			free_message(msg);
			return NULL;
		}

		memcpy(msg->content.data->data, msg_buf + offset, datasize);

		buffer_size = offset + datasize;
	}
	//CT_KEY
	else if(c_type == CT_KEY){
		int keysize;
		memcpy(&keysize, msg_buf + offset, SHORT_SIZE);
		keysize = ntohs(keysize);

		offset += SHORT_SIZE;
		msg->content.key = (char *) malloc(keysize + 1);

		if (msg->content.key == NULL) {
			free_message(msg);
			return NULL;
		}

		memcpy(msg->content.key, msg_buf + offset, keysize);

		// terminar key
		msg->content.key[keysize] = '\0';

		buffer_size = offset + keysize;
	}
	//CT_KEYS
	else if (c_type == CT_KEYS){
		int num_keys;
		memcpy(&num_keys, msg_buf + offset, INT_SIZE);
		offset += INT_SIZE;
		num_keys = ntohl(num_keys);

		msg->content.keys = (char **) malloc(sizeof(char *) * (num_keys + 1));

		if (msg->content.keys == NULL) {
			free_message(msg);
			return NULL;
		}

		int i;
		for(i = 0; i < num_keys; i++)
		{
			// ler tamanho da key
			int key_size;
			memcpy(&key_size, msg_buf + offset, SHORT_SIZE);
			offset += SHORT_SIZE;
			key_size = ntohs(key_size);

			msg->content.keys[i] = (char *) malloc(sizeof(char) * (key_size + 1));

			// falha ao alocar memoria
			if (msg->content.keys[i] == NULL)
			{
				int j;
				// libertar keys anteriores
				for (j = 0; j < i; j++)
					free(msg->content.keys[j]);

				free(msg->content.keys);
				free(msg);

				return NULL;
			}

			// ler key
			memcpy(msg->content.keys[i], msg_buf + offset, key_size);
			// terminar string
			msg->content.keys[i][key_size] = '\0';

			offset += key_size;
		}

		// terminar array de keys
		msg->content.keys[i] = NULL;

		buffer_size = offset;
	}
	//CT_ENTRY
	else if( c_type == CT_ENTRY )
	{
		// keysize
		int keysize;
		memcpy(&keysize, msg_buf + offset, SHORT_SIZE);
		keysize = ntohs(keysize);

		//key
		offset += SHORT_SIZE;
		char *key = (char*) malloc(keysize + 1);

		if (key == NULL) {
			free_message(msg);
			return NULL;
		}

		memcpy(key, msg_buf + offset, keysize);
		// terminar string
		key[keysize] = '\0';

		//datasize
		int data_size;
		offset += keysize;
		memcpy(&data_size, msg_buf + offset, INT_SIZE);
		data_size = ntohl(data_size);

		//criar entry data
		offset += INT_SIZE;
		void *entry_data = (void *) malloc(data_size);

		if (entry_data == NULL) {
			free_message(msg);
			return NULL;
		}

		memcpy(entry_data, msg_buf + offset, data_size);

		//criar entry
		struct data_t *data = data_create2(data_size, entry_data);

		if (data == NULL) {
			free_message(msg);
			return NULL;
		}

		msg->content.entry = entry_create(key, data);

		if (msg->content.entry == NULL) {
			free_message(msg);
			return NULL;
		}

		//libertar structs auxiliares
		data_destroy(data);
		free(entry_data);
		free(key);

		buffer_size = offset + data_size;
	}
	//C_TYPE invalido
	else {
		free_message(msg);
		return NULL;
	}

	//tamanho da mensagem invalido
	if (buffer_size != msg_size) {
		free_message(msg);
		return NULL;
	}

	return msg;
}


void free_message(struct message_t *message) {
	
	if( message == NULL )
		return;

	int i;
	switch( message->c_type ) {
		case CT_VALUE:
			data_destroy( message->content.data );
			break;
		case CT_ENTRY:
			entry_destroy( message->content.entry );
			break;
		case CT_KEY:
			free( message->content.key );
			break;
		case CT_KEYS:
			for ( i = 0; message->content.keys[i] != NULL; i++ )
				free( message->content.keys[i] );

			free( message->content.keys );
			break;
	}

	free( message );
}

int validate_msg(struct message_t *msg) {

	if (validate_opcode(msg->opcode) < 0)
		return -1;

	int valid_msg = -1;

	switch(msg->c_type) {
		case CT_RESULT:
			valid_msg = 0;
			break;
		case CT_VALUE:
			valid_msg = validate_data(msg->content.data);
			break;
		case CT_KEY:
			valid_msg = msg->content.key != NULL;
			break;
		case CT_KEYS:
			valid_msg = msg->content.keys != NULL;
			break;
		case CT_ENTRY:
			valid_msg = validate_entry(msg->content.entry);
			break;
	}

	return valid_msg;
}

int validate_opcode(int opcode) {
	if (opcode == OC_SIZE || opcode == OC_DEL || opcode == OC_UPDATE ||
		opcode == OC_GET || opcode == OC_PUT)
		return 0;

	return -1;
}

int validate_data(struct data_t *data) {
	if (data == NULL || data->datasize < 0 || data->data == NULL)
		return -1;

	return 0;
}

int validate_entry(struct entry_t *entry) {
	if (entry == NULL || entry->key == NULL || validate_data(entry->value) < 0)
		return -1;

	return 0;
}

