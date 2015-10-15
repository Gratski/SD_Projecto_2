#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "table-private.h"

int hashcode(struct table_t *table, char *key){
	int i, val = 0;
	int key_length = strlen(key);

	//se key tem 6 ou menos letras
	if ( key_length <= 6 )
	{
		for (i = 0; i < key_length; i++)
			val += (int) key[i];
	}
	//se key tem mais que 6 letras
	else
	{
		//soma as 3 primeiras letras
		for (i = 0; i < 3; i++)
			val += (int) key[i];

		//soma as 3 ultimas letras
		for (i = 0; i < 3; i++)
			val += (int) key[key_length - i - 1];
	}

	return val % table->num_places;
}


struct table_t *table_create(int n) {
	if (n <= 0)
		return NULL;

	struct table_t *table = (struct table_t *) malloc(sizeof(struct table_t));

	if (table == NULL)
		return NULL;

	table->size = 0;
	table->num_places = n;
	table->places = (struct list_t **) malloc(sizeof(struct list_t *) * n);

	if (table->places == NULL) {
		free(table);
		return NULL;
	}

	int i;
	for (i = 0; i < n; i++)
		table->places[i] = NULL;

	return table;
}


void table_destroy(struct table_t *table){
	if (table == NULL)
		return;

	int i;

	for (i = 0; i < table->num_places; i++)
		list_destroy(table->places[i]);

	free(table->places);
	free(table);
}


int table_put(struct table_t *table, char *key, struct data_t *value){
	if (table == NULL || key == NULL || value == NULL)
		return -1;

	struct entry_t *entry = entry_create(key, value);
	int pos = hashcode(table, key);
	int res;

	// bucket sem lista
	if (table->places[pos] == NULL)
		table->places[pos] = list_create();

	res = list_add(table->places[pos], entry);

	if(res == 0)
		table->size++;

	entry_destroy(entry);
	return res;
}


int table_update(struct table_t *table, char *key, struct data_t *value){
	if (table == NULL || key == NULL || value == NULL)
		return -1;

	int pos = hashcode(table, key);

	struct entry_t *entry = list_get(table->places[pos], key);

	if (entry == NULL)
		return -1;

	entry->value = data_dup(value);

	if (entry->value == NULL)
		return -1;

	return 0;
}


struct data_t *table_get(struct table_t *table, char *key){
	if (table == NULL || key == NULL)
		return NULL;

	int pos = hashcode(table, key);

	if (table->places[pos] == NULL)
		return NULL;

	struct entry_t *entry = list_get(table->places[pos], key);

	if (entry == NULL)
		return NULL;

	return data_dup(entry->value);
}


int table_del(struct table_t *table, char *key){
	if (table == NULL || key == NULL)
		return -1;

	int pos = hashcode(table, key);
	int res = list_remove(table->places[pos], key);

	// se removeu
	if(res == 0)
	{
		table->size--;

		// se a lista em place ficou vazia
		if(table->places[pos]->size == 0)
			table->places[pos] = NULL;
	}

	return res;
}


int table_size(struct table_t *table){
	if (table == NULL)
		return -1;

	return table->size;
}


char **table_get_keys(struct table_t *table){
	if (table == NULL)
		return NULL;

	int i = 0;
	int num_keys = 0;
	char **keys = (char **) malloc(sizeof(char *) * (table_size(table) + 1));

	for(i = 0; i < table->num_places; i++){
		if(table->places[i] != NULL){
			char **l = list_get_keys(table->places[i]);

			if (l == NULL)
				return NULL;

			int j;
			for(j = 0; l[j] != NULL; j++){
				keys[num_keys] = strdup(l[j]);
				num_keys++;
			}

			list_free_keys(l);
		}
	}

	keys[table_size(table)] = NULL;

	return keys;
}


void table_free_keys(char **keys){
	if (keys == NULL)
		return;

	int i;
	for(i = 0; keys[i] != NULL; i++)
		free(keys[i]);

	free(keys);
}
