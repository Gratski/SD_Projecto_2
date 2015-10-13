#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#define SHORT_SIZE	2
#define INT_SIZE  	4
#define LONG_SIZE 	8

#include "message.h"
#include "entry.h"
#include "data.h"

/* Funcao que permite validar o opcode
 * de uma determinada mensagem
 * Devolve: 0 (OK), -1 (opcode invalido)
 */
int validate_opcode(int opcode);

/* Funcao que permite validar o c_type
 * de uma determinada mensagem
 * Devolve: 0 (OK), -1 (opcode invalido)
 */
int validate_c_type(int c_type);

/* Funcao que permite validar o c_type
 * de uma determinada mensagem
 * Devolve: 0 (OK), -1 (c_type invalido)
 */
int validate_data(struct data_t *data);

/* Funcao que permite validar uma determinada entry
 * Devolve: 0 (OK), -1 (entry invalida)
 */
int validate_entry(struct entry_t *entry);

/* Funcao que permite validar uma determinada message
 * Devolve: 0 (OK), -1 (message invalida)
 */
int validate_msg(struct message_t *msg);

#endif

