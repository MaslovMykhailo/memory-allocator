#include "memory-block.h"

#ifndef MEMORYALLOCATOR_MEMORY_ALLOCATION_H
#define MEMORYALLOCATOR_MEMORY_ALLOCATION_H

static Block * heapStart = nullptr;

size_t align(size_t n);

Block * get_mem_block(word_t *data);

word_t * mem_alloc(size_t size);

word_t * mem_realloc(word_t * data, size_t size);

void mem_free(word_t *data);

#endif //MEMORYALLOCATOR_MEMORY_ALLOCATION_H
