/* 	-- Grupo 3 --
	João Gouveia 	nº 45679
	João Rodrigues	nº 45582
	Pedro Luís 		nº 45588
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "entry.h"
#include "list.h"
#include "list-private.h"

struct person{
	char *name;
	int age;
	int hkj;
	int sd;
	int fsd;
	int fsdd;
};

int main(){

	//keys


	//data structs
	struct person *p1 = (struct person *) malloc(sizeof(struct person));
	p1->age = 26;

	//data - entries
	struct data_t *data1 = data_create2(sizeof(struct person), p1);
	
	struct entry_t *entry = entry_create("Joao", data1);
	struct entry_t *entry2 = entry_dup(entry);

	printf("Data é igual?: %d\n", memcmp(entry->value, entry2->value, entry->value->datasize));
	printf("KEY é igual? : %p %p\n", entry->key, entry2->key);
	printf("Entry é igual?: %d\n", memcmp(entry, entry2, sizeof(struct entry_t)));
	

	entry_destroy(entry);
	entry_destroy(entry2);

	printf("end\n");

}