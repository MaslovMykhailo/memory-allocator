#include <utility>
#include <iostream>
#include "memory-allocation.h"
#include "memory-block.h"
#include "sbrk.h"

//
// bytes alignment and size utils
//

inline size_t align(size_t n) {
    return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

inline size_t get_alloc_size(size_t size) {
    return size + sizeof(Block) - sizeof(std::declval<Block>().data);
}

//
// memory blocks common utils
//

Block * get_mem_block(word_t *data) {
//    return (Block *)((char *)data - sizeof(Block) + sizeof(data));
    return (Block *)((char *)data + sizeof(std::declval<Block>().data) - sizeof(Block));
}

Block * request_mem_from_os(size_t size) {
    auto block = (Block *)sbrk(0);

    if (sbrk(get_alloc_size(size)) == nullptr) {
        std::cerr << "Out of memory exception!\n";
        return nullptr;
    }

    return block;
}

//
// memory block manipulation utils
//

bool can_merge(Block *block) {
    auto nextBlock = get_next(block);
    return nextBlock != nullptr && !is_used(nextBlock);
}

Block * merge(Block *block) {
    auto nextBlock = get_next(block);
    block->header += get_size(nextBlock);
    return block;
}

bool can_split(Block *block, size_t size) {
    return get_size(block) > size;
}

Block * split(Block *block, size_t size) {
//    auto subBlock = block + get_size(block) - size;
    auto subBlock = (Block *)((char*)block + get_size(block) - size + sizeof(std::declval<Block>().data));

    subBlock->header = get_size(block) - size;
    set_used(subBlock, false);

    return block;
}

Block * alloc_on_list(Block *block, size_t size) {
    if (can_split(block, size)) {
        block = split(block, size);
    }

    block->header = size;
    set_used(block, true);

    return block;
}

//
// find empty memory block algorithm
//

Block * first_fit(size_t size) {
    auto block = heapStart;

    while (block != nullptr) {
        if (is_used(block) || get_size(block) < size) {
            block = get_next(block);
            continue;
        }

        return block;
    }

    return nullptr;
}

Block * find_block(size_t size) {
    auto foundBlock = first_fit(size);
    if (foundBlock) {
        return alloc_on_list(foundBlock, size);
    } else {
        return foundBlock;
    }
}

//
//  memory allocation and clear functions
//

word_t * mem_alloc(size_t size) {
    size = align(size);

    // ---------------------------------------------------------
    // 1. Search for an available free block:

    if (auto block = find_block(size)) {
        return block->data;
    }

    // ---------------------------------------------------------
    // 2. If block not found in the free list, request from OS:

    auto block = request_mem_from_os(size);

    block->header = size;
    set_used(block, true);


    // Init heap if need:
    if (heapStart == nullptr) {
        heapStart = block;
    }

    // Return user payload:
    return block->data;
}

word_t * mem_realloc(word_t * data, size_t size) {
    auto newSize = align(size);

    auto block = get_mem_block(data);
    if (block != nullptr) {
        auto oldSize = get_size(block);

        if (newSize == oldSize) return data;

        if (newSize < oldSize) {
            block = split(block, newSize);

            block->header = newSize;
            set_used(block, true);

            return data;
        } else {
            auto nextBlock = get_next(block);

            if (nextBlock != nullptr) {
                if (!is_used(nextBlock) && oldSize + get_size(nextBlock) >= newSize) {
                    merge(block);

                    block->header = newSize;
                    set_used(block, true);

                    return data;
                } // else go to bottom
            } else {
                block->header = newSize;
                set_used(block, true);

                return data;
            }
        }
    }

    auto resData = mem_alloc(newSize);
    auto newBlock = get_mem_block(resData);
    newBlock->data[1] = * data;

    set_used(block, false);

    auto next = get_next(newBlock);

    return resData;

}

void mem_free(word_t *data) {
    auto block = get_mem_block(data);
    if (can_merge(block)) {
        block = merge(block);
    }
    set_used(block, false);
}

//
//  print memory state info utils
//

void print_mem_block(Block * block) {
    auto repeater = is_used(block) ? '*' : '~';
    std::cout << "[";
    for (auto i = get_size(block); i > 0; i--) {
        std::cout << repeater;
    }
    std::cout << "]";
}

void mem_dump(const std::string& message = "") {
    if (!message.empty()) {
        std::cout << "\n" << message << "\n";
    }
    auto block = heapStart;
    while (block != nullptr) {
        print_mem_block(block);
        block = get_next(block);
    }
    std::cout << "\n";
}