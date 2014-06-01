/* Developed by Marcio Brito Barbosa */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

typedef struct filterList {
	char *addr;
	struct filterList *next;
}filterList;

filterList* insertFilterList(char *addr, filterList *begin);
void clearFilterList(filterList *begin);
void printList(filterList *begin);
int filterAddr(filterList *begin, char *addr);

#endif