#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "table-private.h"

int hashcode(struct table_t *table, char *key){
	int i, val = 0;
	int key_length = strlen(key);

	//se tem mais que 6
	if ( key_length <= 6 )
	{
		for (i = 0; i < key_length; ++i)
			val += 0 + key[i];
	}
	//se tem mais que 6 letras
	else
	{
		int i;
		//soma as 3 primeiras
		for (i = 0; i < 3; i++)
			val += (int) key[i];

		//soma as 3 ultimas
		for (i = 3; i > 0; i--)
			val += (int) key[key_length - i];
	}

	return val % table->num_places;

}


struct table_t *table_create(int n) {
	if (n < 0)
		return NULL;

	struct table_t *t = ( struct table_t * ) malloc(sizeof( struct table_t ));
	t->num_places = n;
	t->places = (struct list_t **) malloc( sizeof( struct list_t *) * n );

	int i;
	for (i = 0; i < n; ++i)
		t->places[i] = NULL;

	return t;
}


void table_destroy(struct table_t *table){
	if (table == NULL)
		return;

	int size = table->num_places;
	int i;

	for (i = 0; i < size; ++i)
		list_destroy(table->places[i]);

	free(table);
}


int table_put(struct table_t *table, char *key, struct data_t *value){
	if (table == NULL || key == NULL || value == NULL)
		return -1;

	struct entry_t *entry = entry_create(key, value);
	int pos = hashcode(table, key);
	int res;

	// se o place ainda nao tem nada
	if ( table->places[pos] == NULL )
	{
		table->places[pos] = list_create();
		res = list_add(table->places[pos], entry);
	}
	// se o place jah tem lista
	else
	{
		res = list_add(table->places[pos], entry);
	}

	if( res == 0 )
		table->size++;

	return res;
}


int table_update(struct table_t *table, char *key, struct data_t *value){
	if ( table == NULL || key == NULL || value == NULL )
		return -1;

	//obtem hashcode
	int pos = hashcode(table, key);

	struct entry_t *entry = list_get( table->places[pos] ,key);
	struct data_t *data = data_dup(value);

	if ( data == NULL )
		return -1;

	entry->value = data;

	return 0;
}


struct data_t *table_get(struct table_t *table, char *key){
	if ( table == NULL || key == NULL )
		return NULL;

	int pos = hashcode(table, key);

	if ( table->places[pos] == NULL )
		return NULL;

	struct entry_t *entry = list_get( table->places[pos], key );

	if (entry == NULL)
		return NULL;

	return data_dup(entry->value);

	// return entry_dup( list_get( table->places[pos], key ) );
}


int table_del(struct table_t *table, char *key){
	if ( table == NULL || key == NULL )
		return -1;

	int pos = hashcode(table, key);

	struct list_t *list = table->places[pos];

	int res = list_remove(list, key);

	// se removeu
	if( res == 0 )
	{
		table->size = table->size - 1;

		// se a list em place ficou vazia
		if( list->size == 0 )
		{
			list_destroy(list);
			table->places[pos] = NULL;
		}
	}

	return res;

}


int table_size(struct table_t *table){
	if ( table == NULL )
		return -1;

	return table->size;
}


//TODO
char **table_get_keys(struct table_t *table){
	if (table == NULL)
		return NULL;

	int i = 0;
	int index = 0;
	char **keys = (char **) malloc( (sizeof(char *) * table->size) + 1 );
	for(i=0; i<table->size; i++){

		if( table->places[i] != NULL ){
			char **l = list_get_keys(table->places[i]);
			int j;
			for(j=0; l[j]!=NULL; j++){
				keys[index] = strdup(l[j]);
				index++;
			}

			list_free_keys(l);
		}
	}
	return keys;
}


void table_free_keys(char **keys){
	if (keys == NULL)
		return;

	int i;
	for(i=0; keys[i]!=NULL; i++)
		free(keys[i]);

	free(keys);

}
