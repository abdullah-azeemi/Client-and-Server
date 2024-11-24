#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

void *mymalloc(size_t size);
void myfree(void *ptr);
void print_memory_usage();

#endif 
