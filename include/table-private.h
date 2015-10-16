/* 	-- Grupo 3 --
	João Gouveia 	nº 45679
	João Rodrigues	nº 45582
	Pedro Luís 		nº 45588
*/

#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "table.h"

struct table_t{
	int size;
	int num_places;
	struct list_t **places;
};

/* Funcao que permite obter o hashcode
 * de uma determinada key
 * Devolve: 0 <= hashcode < numero de buckets (OK)
 * 			-1 (em caso de erro)
 */
int hashcode(struct table_t *table, char *key);

#endif