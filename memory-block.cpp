#include <iostream>
#include "memory-block.h"

size_t get_size(Block *block) {
    return block->header & ~1L;
}

bool is_used(Block *block) {
    return block->header & 1;
}

void set_used(Block *block, bool used) {
    if (used) {
        block->header |= 1;
    } else {
        block->header &= ~1;
    }
}

Block * get_next(Block * block) {
    auto nextBlock = (Block *)((char*)block + get_size(block) + sizeof(std::declval<Block>().data));
    return get_size(nextBlock) > 0 ? nextBlock : nullptr;
}
