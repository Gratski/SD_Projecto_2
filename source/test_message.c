#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>
#include "message.h"
#include "list.h"
#include "data.h"
#include "entry.h"

void print_message(struct message_t *msg) {
	int i;

	printf("----- MESSAGE -----\n");
	printf("opcode: %d, c_type: %d\n",msg->opcode,msg->c_type);
	switch(msg->c_type) {
		case CT_ENTRY:{
			printf("key: %s\n",msg->content.entry->key);
			printf("datasize: %d\n",msg->content.entry->value->datasize);
		}break;
		case CT_KEY:{
			printf("key: %s\n",msg->content.key);
		}break;
		case CT_KEYS:{
			for(i=0; msg->content.keys[i] !=NULL; i++) {
				printf("key[%d]: %s\n",i,msg->content.keys[i]);
			}
		}break;
		case CT_VALUE:{
			printf("datasize: %d\n",msg->content.data->datasize);
		}break;
		case CT_RESULT:{
			printf("result: %d\n",msg->content.result);
		};
	}
	printf("-------------------\n");
}

char** create_keys() {
	struct list_t *list = list_create();
	char **keys;
	struct data_t *data;
	struct entry_t *entry;

	char *key1 = "abc";
	char *key2 = "123";
	char *key3 = "ul2012";
	char *key4 = "TESTE";

	data = data_create2(strlen(key1)+1,key1);
	entry = entry_create(key1, data);
	list_add(list,entry);

	data = data_create2(strlen(key2)+1,key2);
	entry = entry_create(key2,data);
	list_add(list,entry);

	data = data_create2(strlen(key3)+1,key3);
	entry = entry_create(key3,data);
	list_add(list,entry);

	data = data_create2(strlen(key4)+1,key4);
	entry = entry_create(key4,data);
	list_add(list,entry);

	//free(key1);
	//free(key2);
	//free(key3);
	//free(key4);
	free(data);
	free(entry);
	keys = list_get_keys(list);
	list_destroy(list);

	return keys;
}

int testResult() {
	int result, size;
	char *msg_str = NULL;
	struct message_t *msg = (struct message_t*)malloc(sizeof(struct message_t));
	msg->opcode = OC_PUT;
	msg->c_type = CT_RESULT;
	msg->content.result = 1;
	size = message_to_buffer(msg,&msg_str);

	int opcode = htons(msg->opcode);
	int c_type = htons(msg->c_type);
	int res = htonl(msg->content.result);

	result = (memcmp(msg_str, &opcode, 2) == 0 &&
		 memcmp(msg_str+2, &c_type, 2) == 0 &&
		 memcmp(msg_str+4, &res, 4) == 0);

	free_message(msg);

	msg = buffer_to_message(msg_str, size);

	result = result && (msg->opcode == OC_PUT &&
			    msg->c_type == CT_RESULT &&
			    msg->content.result == 1);

	free(msg_str);
	print_message(msg);
	free_message(msg);

	printf("Modulo mensagem -> teste - Result: %s\n",result?"passou":"nao passou");
	return result;
}

int testKey() {
	int result, size;
	char *msg_str = NULL;
	struct message_t *msg = (struct message_t*)malloc(sizeof(struct message_t));
	msg->opcode = OC_PUT;
	msg->c_type = CT_KEY;
	msg->content.key = strdup("abcdef");
	size = message_to_buffer(msg,&msg_str);

	int opcode = htons(msg->opcode);
	int c_type = htons(msg->c_type);
	int keysize = strlen(msg->content.key);
	int keysize_conv = htons(keysize);
	char comp_key[keysize];
	memcpy(comp_key, msg_str+6, keysize);

	result = (memcmp(msg_str, &opcode, 2) == 0 &&
		  	  memcmp(msg_str+2, &c_type, 2) == 0 &&
		 	  memcmp(msg_str+4, &keysize_conv, 2) == 0 &&
		 	  memcmp(msg_str+6, &comp_key, keysize) == 0);

	free_message(msg);

	msg = buffer_to_message(msg_str, size);

	result = result && (msg->opcode == 50 &&
			    msg->c_type == CT_KEY &&
			    strcmp(msg->content.key,"abcdef") == 0);

	free(msg_str);
	print_message(msg);
	free_message(msg);

	printf("Modulo mensagem -> teste - Key: %s\n",result?"passou":"nao passou");
	return result;
}

int testValue() {
	int result, size;
	char *msg_str = NULL;
	struct message_t *msg = (struct message_t*) malloc(sizeof(struct message_t));
	msg->opcode = OC_PUT;
	msg->c_type = CT_VALUE;
	msg->content.data = data_create2(strlen("abc")+1, "abc");

	size = message_to_buffer(msg,&msg_str);

	int opcode = htons(msg->opcode);
	int c_type = htons(msg->c_type);
	int datasize = msg->content.data->datasize;
	int datasize_conv = htonl(datasize);
	char *comp_data = (char *) malloc(datasize);
	memcpy(comp_data, msg_str+8, datasize);

	result = (memcmp(msg_str, &opcode, 2) == 0 &&
		  	  memcmp(msg_str+2, &c_type, 2) == 0 &&
		 	  memcmp(msg_str+4, &datasize_conv, 4) == 0 &&
		 	  memcmp(msg_str+8, comp_data, datasize) == 0);

	free(comp_data);
	free_message(msg);

	msg = buffer_to_message(msg_str, size);

	result = result && (msg->opcode == OC_PUT &&
			    msg->c_type == CT_VALUE &&
			    msg->content.data->datasize == strlen("abc")+1 &&
			    strcmp(msg->content.data->data,"abc") == 0);

	free(msg_str);
	print_message(msg);
	free_message(msg);
	printf("Modulo mensagem -> teste - Value: %s\n",result?"passou":"nao passou");

	return result;
}

int testEntrySemTS() {
	int result, size;
	char *msg_str = NULL;
	struct message_t *msg = (struct message_t*)malloc(sizeof(struct message_t));
	msg->opcode = OC_PUT;
	msg->c_type = CT_ENTRY;
	msg->content.entry = entry_create(strdup("abc"),data_create2(strlen("abc")+1,strdup("abc")));
	size = message_to_buffer(msg,&msg_str);

	int opcode = htons(msg->opcode);
	int c_type = htons(msg->c_type);
	int keysize = strlen(msg->content.entry->key);
	int keysize_conv = htonl(keysize);
	char comp_key[keysize];
	memcpy(comp_key, msg_str+8, keysize);
	int datasize = msg->content.entry->value->datasize;
	int datasize_conv = htonl(datasize);
	char comp_data[datasize];
	memcpy(comp_data, msg_str+8+keysize+8+4, datasize);
	result = (memcmp(msg_str, &opcode, 2) == 0 &&
		  	  memcmp(msg_str+2, &c_type, 2) == 0 &&
			  memcmp(msg_str+4+8, &keysize_conv, 4) == 0 &&
			  memcmp(msg_str+4+8+4, &comp_key, keysize) == 0 &&
		 	  memcmp(msg_str+4+8+4+keysize, &datasize_conv, 4) == 0 &&
		 	  memcmp(msg_str+4+8+4+keysize+4, &comp_data, datasize) == 0);

	free_message(msg);

	printf("!!!result: %d\n", result);

	msg = buffer_to_message(msg_str, size);

	result = result && (msg->opcode == OC_PUT &&
			    msg->c_type == CT_ENTRY &&
			    strcmp(msg->content.entry->key,"abc") == 0 &&
			    msg->content.entry->value->datasize == strlen("abc")+1 &&
			    strcmp(msg->content.entry->value->data,"abc") == 0);

	free(msg_str);

	print_message(msg);

	free_message(msg);

	printf("Modulo mensagem -> teste - Entry sem TS: %s\n",result?"passou":"nao passou");
	return result;
}

int testEntry() {
	int result, size;
	char *msg_str = NULL;
	struct message_t *msg = (struct message_t*) malloc(sizeof(struct message_t));
	msg->opcode = OC_PUT;
	msg->c_type = CT_ENTRY;

	char *key = "abc";
	char *data_s = "abc";

	struct data_t *aux_data = data_create2(strlen(data_s)+1, data_s);

	msg->content.entry = entry_create(key, aux_data);

	data_destroy(aux_data);

	size = message_to_buffer(msg,&msg_str);

	int opcode = htons(msg->opcode);
	int c_type = htons(msg->c_type);
	int keysize = strlen(msg->content.entry->key);
	int keysize_conv = htons(keysize);
	char *comp_key = (char *) malloc(keysize);
	memcpy(comp_key, msg_str + 6, keysize);

	int datasize = msg->content.entry->value->datasize;
	int datasize_conv = htonl(datasize);
	char *comp_data = (char *) malloc(datasize);

	memcpy(comp_data, msg_str + 4 + 2 + keysize + 4, datasize);

	result = (memcmp(msg_str, &opcode, 2) == 0 &&
		  	  memcmp(msg_str+2, &c_type, 2) == 0 &&
			  memcmp(msg_str+2+2, &keysize_conv, 2) == 0 &&
			  memcmp(msg_str+2+2+2, comp_key, keysize) == 0 &&
		 	  memcmp(msg_str+2+2+2+keysize, &datasize_conv, 4) == 0 &&
		 	  memcmp(msg_str+2+2+2+keysize+4, comp_data, datasize) == 0);

	free(comp_key);
	free(comp_data);
	free_message(msg);

	// TODO corrigir alocacoes de memoria na parte relativa a CT_ENTRY.
	msg = buffer_to_message(msg_str, size);


	result = result && (msg->opcode == OC_PUT &&
			    msg->c_type == CT_ENTRY &&
			    strcmp(msg->content.entry->key,"abc") == 0 &&
			    msg->content.entry->value->datasize == strlen("abc")+1 &&
			    strcmp(msg->content.entry->value->data,"abc") == 0);

	free(msg_str);
	print_message(msg);
	free_message(msg);

	printf("Modulo mensagem -> teste - Entry: %s\n",result?"passou":"nao passou");
	return result;
}

int testKeys() {
	int result, size;
	char *msg_str = NULL;
	struct message_t *msg = (struct message_t*)malloc(sizeof(struct message_t));
	msg->opcode = OC_PUT;
	msg->c_type = CT_KEYS;
	msg->content.keys = create_keys();
	size = message_to_buffer(msg,&msg_str);

	int opcode = htons(msg->opcode);
	int c_type = htons(msg->c_type);
	int num_keys = 4;
	int num_keys_conv = htonl(num_keys);
	int size1 = strlen("123");
	int size2 = strlen("abc");
	int size3 = strlen("TESTE");
	int size4 = strlen("ul2012");
	int size1_c = htonl(size1);
	int size2_c = htonl(size2);
	int size3_c = htonl(size3);
	int size4_c = htonl(size4);

	result = (memcmp(msg_str, &opcode, 2) == 0 &&
		  	  memcmp(msg_str+2, &c_type, 2) == 0 &&
		 	  memcmp(msg_str+4, &num_keys_conv, 4) == 0 &&
			  memcmp(msg_str+8, &size1_c, 2) == 0 &&
			  memcmp(msg_str+12+size1, &size2_c, 2) == 0 &&
			  memcmp(msg_str+12+2+size1 + size2, &size3_c, 2) == 0 &&
			  memcmp(msg_str+12+4+size1 + size2 + size3, &size4_c, 2) == 0);

	free_message(msg);

	msg = buffer_to_message(msg_str, size);

	result = result && (msg->opcode == OC_PUT &&
			    msg->c_type == CT_KEYS &&
			    strcmp(msg->content.keys[0],"123") == 0 &&
			    strcmp(msg->content.keys[1],"abc") == 0 &&
			    strcmp(msg->content.keys[2],"TESTE") == 0 &&
			    strcmp(msg->content.keys[3],"ul2012") == 0 &&
			    msg->content.keys[4] == NULL);

	free(msg_str);
	print_message(msg);
	free_message(msg);

	printf("Modulo mensagem -> teste - Keys: %s\n",result?"passou":"nao passou");
	return result;
}

int testInvalida() {
	int result;
	char *msg_lixada1 = "1234 50 abcederdfasdasfsdfafadgadgagadgadhgdfhsghsshg";
	struct message_t *msg;

	msg = buffer_to_message(msg_lixada1, strlen(msg_lixada1)+1);

	result = (msg == NULL);

	printf("Modulo mensagem -> teste - Invalida: %s\n",result?"passou":"nao passou");
	return result;
}

int main() {
	int score = 0;

	printf("Iniciando o teste do modulo message\n");

	score += testResult();

	score += testValue();

	score += testKey();

	score += testEntry();

	//TODO corrigir ct_keys
	score += testKeys();

	score += testInvalida();


	printf("Resultados do teste do modulo message: %d/6\n",score);

	return score;
}
