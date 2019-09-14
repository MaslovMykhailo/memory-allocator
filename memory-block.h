#ifndef MEMORYALLOCATOR_MEMORY_BLOCK_H
#define MEMORYALLOCATOR_MEMORY_BLOCK_H

using word_t = int;

struct Block {
    // Object header
    size_t header;
    // User data
    word_t data[1];
};

size_t get_size(Block *block);

bool is_used(Block *block);

void set_used(Block *block, bool used);

Block * get_next(Block * block);

#endif //MEMORYALLOCATOR_MEMORY_BLOCK_H
