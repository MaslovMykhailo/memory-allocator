#ifndef BUDDY_ALLOCATION_MEMORYALLOCATOR_H
#define BUDDY_ALLOCATION_MEMORYALLOCATOR_H


#include "BlocksList.h"

#define MEMORY_DEFAULT_SIZE_KB 1024
#define BLOCK_MIN_SIZE 64

struct Block {
    char* startAddress;
    unsigned int size;
};

enum Measure {
    BYTE = 1,
    K_BYTE = 1000,
    M_BYTE = 1000000
};

class MemoryAllocator {
private:
    unsigned long _size;
    unsigned long _listsCount;
    char *_memory;
    BlocksList *_blocks;
    Measure _measure;

    void init(int size, Measure measure);

    int getListsCount();
    static int getListIndex(int size);

    void sortList(int listIndex);
    int *findBuddies(int listIndex);

    void merge(int listIndex);
    void mergeAll();

public:
    MemoryAllocator();
    MemoryAllocator(int sizeKb, Measure measure);

    static unsigned long calcSize(int size, Measure measure);
    static int calcBlockSize(int index);

    unsigned long getSize();
    unsigned long getMeasuredSize();

    void dump();
    char *getMemoryPointer();

    Block *alloc(int size);
    void free(Block *freeBlock);
};


#endif //BUDDY_ALLOCATION_MEMORYALLOCATOR_H
