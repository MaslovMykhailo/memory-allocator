#include <iostream>
#include <cassert>
#include <sys/mman.h>
#include "sbrk.h"

/**
 * Machine word size. Depending on the architecture,
 * can be 4 or 8 bytes.
 */
using word_t = int; // or intptr_t for best fit in each architecture

/**
 * Allocated block of memory. Contains the object header structure,
 * and the actual payload pointer.
 */
struct Block {

    // -------------------------------------
    // 1. Object header

    /**
     * Block size.
     */
    size_t size;

    /**
     * Next block in the list.
     */
    Block *next;

    /**
     * Whether this block is currently used.
     */
    bool used;

    // -------------------------------------
    // 2. User data

    /**
     * Payload pointer.
     */
    word_t data[1];

};

/**
 * Heap start. Initialized on first allocation.
 */
static Block *heapStart = nullptr;

/**
 * Current top. Updated on each allocation.
 */
static auto top = heapStart;

/**
 * Aligns the size by the machine word.
 */
inline size_t align(size_t n) {
    return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

/**
 * Returns total allocation size, reserving in addition the space for
 * the Block structure (object header + first data word).
 *
 * Since the `word_t data[1]` already allocates one word inside the Block
 * structure, we decrease it from the size request: if a user allocates
 * only one word, it's fully in the Block struct.
 */
inline size_t allocSize(size_t size) {
    return size + sizeof(Block) - sizeof(std::declval<Block>().data);
}

/**
 * Requests (maps) memory from OS.
 */
Block *requestFromOS(size_t size) {
    // Current heap break.
    auto block = (Block *)sbrk(0);

    // OOM.
    if (sbrk(allocSize(size)) == (void *)-1) {
        return nullptr;
    }

    return block;
}

Block *getHeader(word_t *data) {
    return (Block *)((char *)data + sizeof(std::declval<Block>().data) - sizeof(Block));
}

/**
 * Whether we should merge this block.
 */
bool canCoalesce(Block *block) {
    return block->next && !block->next->used;
}

/**
 * Coalesces two adjacent blocks.
 */
Block *coalesce(Block *block) {
    auto nextBlock = block->next;
    block->next = nextBlock->next;
    block->size = block->size + nextBlock->size;
    delete(nextBlock);
    return block;
}

/**
 * Frees the previously allocated block.
 */
void free(word_t *data) {
    auto block = getHeader(data);
    if (canCoalesce(block)) {
        block = coalesce(block);
    }
    block->used = false;
}

/**
 * First-fit algorithm.
 *
 * Returns the first free block which fits the size.
 */
Block *firstFit(size_t size) {
    auto block = heapStart;

    while (block != nullptr) {
        // O(n) search.
        if (block->used || block->size < size) {
            block = block->next;
            continue;
        }

        // Found the block:
        return block;
    }

    return nullptr;
}

/**
 * Splits the block on two, returns the pointer to the smaller sub-block.
 */
Block *split(Block *block, size_t size) {
    auto nextBlock = block->next;
    auto splittedBlock = new Block();

    splittedBlock->next = nextBlock;
    splittedBlock->size = block->size - size;
    splittedBlock->used = false;

    block->next = splittedBlock;

    return block;
}

/**
 * Whether this block can be split.
 */
inline bool canSplit(Block *block, size_t size) {
    return block->size > size;
}

/**
 * Allocates a block from the list, splitting if needed.
 */
Block *listAllocate(Block *block, size_t size) {
    // Split the larger block, reusing the free part.
    if (canSplit(block, size)) {
        block = split(block, size);
    }

    block->size = size;
    block->used = true;

    return block;
}

/**
 * Tries to find a block of a needed size.
 */
Block *findBlock(size_t size) {
    auto foundBlock = firstFit(size);
    if (foundBlock) {
        return listAllocate(foundBlock, size);
    } else {
        return foundBlock;
    }
}

/**
 * Allocates a block of memory of (at least) `size` bytes.
 */
word_t *alloc(size_t size) {
    size = align(size);

    // ---------------------------------------------------------
    // 1. Search for an available free block:

    if (auto block = findBlock(size)) {                   // (1)
        return block->data;
    }

    // ---------------------------------------------------------
    // 2. If block not found in the free list, request from OS:

    auto block = requestFromOS(size);

    block->size = size;
    block->used = true;

    // Init heap.
    if (heapStart == nullptr) {
        heapStart = block;
    }

    // Chain the blocks.
    if (top != nullptr) {
        top->next = block;
    }

    top = block;

    // User payload:
    return block->data;
}




int main(int argc, char const *argv[]) {
    init();

    assert(align(3) == 4);
    assert(align(8) == 8);
    assert(align(12) == 12);
    assert(align(16) == 16);

    // --------------------------------------
    // Test case 1: Alignment
    //
    // A request for 3 bytes is aligned to 4.
    //

    auto p1 = alloc(3);
    auto p1b = getHeader(p1);
    assert(p1b->size == sizeof(word_t));


    // --------------------------------------
    // Test case 2: Exact amount of aligned bytes
    //

    auto p2 = alloc(8);                        // (2)
    auto p2b = getHeader(p2);
    assert(p2b->size == 8);


    // --------------------------------------
    // Test case 3: Free the object
    //

    free(p2);
    assert(!p2b->used);

    // Test case 4: The block is reused
    //
    // A consequent allocation of the same size reuses
    // the previously freed block.
    //

    auto p3 = alloc(8);
    auto p3b = getHeader(p3);
    assert(p3b->size == 8);
    assert(p3b == p2b);  // Reused!


    // Test case 5: The block is splitted

    auto p4 = alloc(16);
    auto p4b = getHeader(p4);
    free(p4);
    auto p5 = alloc(8);
    auto p5b = getHeader(p5);
    assert(p5b->size == 8);
    assert(p4b == p5b);  // Reused part of splitted block!
    assert(p5b->next != nullptr);
    assert(!p5b->next->used);
    auto p6 = alloc(2);
    auto p6b = getHeader(p6);
    assert(p6b->size == 4);
    assert(p5b->next == p6b);

    // Test case 5: Blocks coalescing

    assert(p6b->next && !p6b->next->used);
    free(p6);
    assert(!p6b->next);
    assert(p6b->size == 8);

    puts("\nAll assertions passed!\n");
}