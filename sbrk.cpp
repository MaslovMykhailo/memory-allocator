#include "sbrk.h"
#include <sys/mman.h>
#define MAX_HEAP 4194304 // 4 MB

static char * heap ;
char * brkp = nullptr;
char * endp = nullptr;

void init_heap( ) {
    heap = (char *)mmap(nullptr, MAX_HEAP, (PROT_READ | PROT_WRITE), (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    brkp = heap;
    endp = brkp + MAX_HEAP;
}

void * sbrk(size_t size) {
    if (size == 0) {
        return (void*)brkp;
    }
    void *free = (void*)brkp;
    brkp += size;
    if (brkp >= endp) {
        return nullptr;
    }
    return free;
}
