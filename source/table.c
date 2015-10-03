#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "table-private.h"

struct table_t *table_create(int n) {
	
	struct table_t *t = ( struct table_t * ) malloc(sizeof( struct table_t ));
	t->num_places = n;
	t->places = (struct list_t **) malloc( sizeof( struct list_t *) * n );
	int i;
	for (i = 0; i < n; ++i)
	{
		t->places[i] = NULL;
	}
	return t;
}
