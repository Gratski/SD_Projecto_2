/* 	-- Grupo 3 --
	João Gouveia 	nº 45679
	João Rodrigues	nº 45582
	Pedro Luís 		nº 45588
*/

#include "list-private.h"
#include "entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list_t *list_create(){
	struct list_t *list = (struct list_t *) malloc(sizeof(struct list_t));

	// se nao tem espaco de memoria
	if( list == NULL )
		return NULL;

	list->size = 0;
	list->head = NULL;

	return list;
}


void list_destroy(struct list_t *list){
	if(list == NULL)
		return;

	struct node_t *cur = list->head;
	struct node_t *aux;

	while(cur != NULL){
		aux = cur;
		cur = cur->next;
		entry_destroy(aux->entry);
		free(aux);
	}

	free(list);
}


int list_add(struct list_t *list, struct entry_t *entry){
	if (list == NULL || entry == NULL)
		return -1;

	struct node_t* node = (struct node_t *) malloc(sizeof (struct node_t));

	if (node == NULL)
		return -1;

	// preparar node a ser inserido na lista
	node->entry = entry_dup(entry);
	node->next = NULL;

	struct node_t *current  = list->head;
	struct node_t *previous = current;
	int str_cmp;

	// lista vazia
	if (list->head == NULL) {
		list->head = node;
		list->size++;
	}
	// adicionar no ao inicio da lista se nao existe
	else if (strcmp(entry->key, list->head->entry->key) < 0) {
		node->next = list->head;
		list->head = node;
		list->size++;
	}
	else {
		str_cmp = strcmp(entry->key, current->entry->key);

		// encontrar posicao que a key excede
		while( str_cmp > 0 && current->next != NULL )
		{
			//printf("str_cmp: %d\n", strcmp( entry->key, current->entry->key ));
			//printf("entry: %s, current :%s\n", entry->key, current->entry->key);

			previous = current;
			current  = current->next;
			str_cmp  = strcmp( entry->key, current->entry->key );
		}

		// key jah presente na lista
		if ( str_cmp == 0 ){
			return -1;
		}
		// adicionar no antes do no encontrado
		else if (str_cmp < 0) {
			//printf("entry: %s > current :%s\n", entry->key, current->entry->key);
			previous->next = node;
			node->next = current;
			list->size++;
		}
		// adicionar no depois do no encontrado
		// str_cmp > 0
		else {
			//printf("entry: %s > current :%s\n", entry->key, current->entry->key);
			current->next = node;
			list->size++;
		}
	}

	return 0;
}


int list_remove(struct list_t *list, char *key){
	// se estah vazia ou nao foi alocada
	if ( list == NULL || list->size == 0 || key == NULL)
		return -1;

	struct node_t *cur = list->head;
	struct node_t *auxPrev = NULL;		// aponta para o indice anterior ao actual

	while( cur != NULL )
	{
		if( strcmp(key, cur->entry->key) == 0 )
		{
			// se nao estah na 1a posicao
			if( auxPrev != NULL )
				auxPrev->next = cur->next;
			else
				list->head = cur->next;

			list->size--;
			entry_destroy(cur->entry);
			free(cur);

			return 0;
		}

		auxPrev = cur;
		cur = cur->next;

	}

	// se nao encontrou
	return -1;
}


struct entry_t *list_get(struct list_t *list, char *key){
	if (list == NULL || key == NULL)
		return NULL;

	struct node_t *cur = list->head;

	// percorre a lista
	while(cur != NULL)
	{
		// verifica se eh o que deve retornar
		if( strcmp( cur->entry->key, key ) == 0 )
			return cur->entry;

		cur = cur->next;
	}

	// caso nao tenha encontrado nada
	return NULL;
}


int list_size(struct list_t *list){
	// se erro
	if( list == NULL )
		return -1;
	// se OK devolve size
	else
		return list->size;
}


char **list_get_keys(struct list_t *list){
	if (list == NULL)
		return NULL;

	// array com tamanho para todos os pointers para strings
	char **arr = (char **) malloc(sizeof(char *) * (list_size(list) + 1));

	if (arr == NULL)
		return NULL;

	// escreve um null no final para servir de limiter
	arr[list_size(list)] = NULL;

	//primeiro node da lista
	struct node_t *cur = list->head;
	int i;

	//ciclo para obter as keys da lista
	for(i = 0; cur != NULL; i++)
	{
		// escreve em indice "i" a current entry key
		arr[i] = strdup(cur->entry->key);

		// falha ao alocar memoria para uma key
		if (arr[i] == NULL) {
			int j;
			// libertar memoria alocada pelas keys anteriores
			for (j = 0; j < i; j++)
				free(arr[j]);

			return NULL;
		}

		//avança pointer de lista
		cur = cur->next;
	}

	return arr;
}


void list_free_keys(char **keys){
	if (keys == NULL)
		return;

	int i = 0;
	// percorre ateh ao final do array
	for(i = 0; keys[i] != NULL; i++)
		free(keys[i]);

	// liberta memoria de array
	free(keys);
}


/*
	faz o print da lista ligada
	util para debug
*/
void list_print(struct list_t *list){
	if (list == NULL)
		return;

	struct node_t *cur = list->head;

	while(cur != NULL)
	{
		printf("%s\n", cur->entry->key);
		cur = cur->next;
	}
}





