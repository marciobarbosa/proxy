/* Developed by Marcio Brito Barbosa */

#include <stdio.h>
#include <stdlib.h>
#include "../hdr/util.h"

filterList* insertFilterList(char *addr, filterList *begin) {
	filterList *new_element = (filterList*)calloc(1, sizeof(filterList));
	filterList *current = begin;

	new_element->addr = (char*)calloc(strlen(addr) + 1, sizeof(char));
	memset(new_element->addr, 0, strlen(addr) + 1);
	strcpy(new_element->addr, addr);
	new_element->addr[strlen(addr)] = '\0';
	new_element->next = NULL;

	if(begin == NULL) 
		begin = new_element;
	else {
		while(current->next != NULL)
			current = current->next;

		current->next = new_element;
	}

	return begin;
}

void clearFilterList(filterList *begin) {
	filterList *aux = begin, *former = begin;

	while(aux != NULL) {
		former = aux;
		aux = aux->next;
		free(former);
	}
}

void printList(filterList *begin) {
	filterList *aux = begin;

	while(aux != NULL) {
		printf("%s; ", aux->addr);
		aux = aux->next;
	}
	printf("\n");
}

int filterAddr(filterList *begin, char *addr) {
	filterList *aux = begin;
	int return_value = FALSE;

	while(begin != NULL) {
		if(strstr(begin->addr, addr) != NULL || strstr(addr, begin->addr) != NULL) {
			return_value = TRUE;
			break;
		}
		begin = begin->next;
	}

	return return_value;
}