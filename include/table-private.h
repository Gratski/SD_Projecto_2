#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

struct table_t{
	int size;
	int num_places;
	struct list_t **places;
};

int hashcode(struct table_t *table, char *key);

#endif