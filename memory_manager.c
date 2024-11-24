#include "memory_manager.h"
#include <stdio.h>
#include <stddef.h>

#define ARENA_SIZE 1024 * 1024  // 1 MB

static char arena[ARENA_SIZE];  // The memory arena
static size_t arena_offset = 0; // Tracks the current position in the arena

typedef struct FreeBlock {
    size_t size;              // Size of the free block
    struct FreeBlock *next;   // Pointer to the next free block
} FreeBlock;

static FreeBlock *free_list = NULL; // List of free blocks for `myfree`

void *mymalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Align size to 8 bytes for proper alignment
    size = (size + 7) & ~7;

    // Check free list for a reusable block
    FreeBlock *prev = NULL, *current = free_list;
    while (current) {
        if (current->size >= size) {  // Found a suitable block
            if (current->size > size + sizeof(FreeBlock)) {
                // Split the block if it's significantly larger
                FreeBlock *new_block = (FreeBlock *)((char *)current + sizeof(FreeBlock) + size);
                new_block->size = current->size - size - sizeof(FreeBlock);
                new_block->next = current->next;
                if (prev) {
                    prev->next = new_block;
                } else {
                    free_list = new_block;
                }
                current->size = size;
            } else {  // Use the whole block
                if (prev) {
                    prev->next = current->next;
                } else {
                    free_list = current->next;
                }
            }
            return (char *)current + sizeof(FreeBlock);
        }
        prev = current;
        current = current->next;
    }

    // If no suitable block found, allocate from the arena
    if (arena_offset + size + sizeof(FreeBlock) > ARENA_SIZE) {
        return NULL;  // Out of memory
    }

    void *ptr = arena + arena_offset;
    arena_offset += size + sizeof(FreeBlock);
    return (char *)ptr + sizeof(FreeBlock);
}

void myfree(void *ptr) {
    if (!ptr) return;

    FreeBlock *block = (FreeBlock *)((char *)ptr - sizeof(FreeBlock));
    block->next = free_list;
    free_list = block;
}

void print_memory_usage() {
    printf("Memory used: %zu bytes\n", arena_offset);
    printf("Free blocks:\n");
    FreeBlock *current = free_list;
    while (current) {
        printf("- Block of size %zu bytes\n", current->size);
        current = current->next;
    }
}
