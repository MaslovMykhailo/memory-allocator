#include <iostream>
#include "memory-block.h"

size_t get_size(Block *block) {
    // get all without least-significant bit
    return block->header & ~1uL;
}

bool is_used(Block *block) {
    // get least-significant bit
    return block->header & 1u;
}

void set_used(Block *block, bool used) {
    // set 1 | 0 to least-significant bit
    if (used) {
        block->header |= 1u;
    } else {
        block->header &= ~1u;
    }
}

Block * get_next(Block * block) {
    auto nextBlock = (Block *)((char*)block + get_size(block) + sizeof(std::declval<Block>().data));
    return get_size(nextBlock) > 0 ? nextBlock : nullptr;
}
