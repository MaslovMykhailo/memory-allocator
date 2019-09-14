#ifndef MEMORYALLOCATOR_SBRK_H
#define MEMORYALLOCATOR_SBRK_H

#endif //MEMORYALLOCATOR_SBRK_H

#include <sys/mman.h>

void init();
void * sbrk(size_t size);
