#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "entry.h"
#include "list.h"
#include "message.h"
#include "table.h"

int main(){
	
	struct message_t *msg = (struct message_t *) malloc(sizeof(struct message_t));
	msg->opcode = OC_PUT;
	msg->c_type = CT_RESULT;
	msg->content.result = 1120;
	
	char *buff;
	int size;
	size = message_to_buffer(msg, &buff);
	struct message_t *msg2 = buffer_to_message(buff, size);
	printf("OPCODE: %d\n", msg2->opcode);
	printf("C_TYPE: %d\n", msg2->c_type);
	
	free(msg);

	return 0;
}
