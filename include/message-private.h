#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#define SHORT_SIZE	2
#define INT_SIZE  	4
#define LONG_SIZE 	8

/* Funcao que permite validar o opcode
 * de uma determinada mensagem
 */
int validate_opcode(int opcode);

/* Funcao que permite validar o c_type
 * de uma determinada mensagem
 */
int validate_data(struct data_t *data);

/* Funcao que permite validar uma determinada entry
 */
int validate_entry(struct entry_t *entry);

/* Funcao que permite validar uma determinada message
 */
int validate_msg(struct message_t *msg);

#endif

