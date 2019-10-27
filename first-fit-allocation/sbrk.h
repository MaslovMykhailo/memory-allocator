#include <sys/mman.h>

#ifndef MEMORYALLOCATOR_SBRK_H
#define MEMORYALLOCATOR_SBRK_H

void init_heap();
void * sbrk(size_t size);

#endif //MEMORYALLOCATOR_SBRK_H
