#include <iostream>
#include <cassert>
#include "sbrk.h"
#include "memory-block.h"
#include "memory-allocation.h"

int main(int argc, char const *argv[]) {
    init_heap();

    // --------------------------------------
    // Test case 1: Alignment
    //

    assert(align(3) == 8);
    assert(align(8) == 8);
    assert(align(12) == 16);
    assert(align(16) == 16);
    assert(align(20) == 24);
    assert(align(128 - 32) == 96);

    auto p1 = mem_alloc(1);
    mem_dump("Operation 1: Allocate 1 byte");
    auto p1b = get_mem_block(p1);
    assert(get_size(p1b) == sizeof(word_t));


    // --------------------------------------
    // Test case 2: Exact amount of aligned bytes
    //

    auto p2 = mem_alloc(8);
    mem_dump("Operation 2: Allocate 8 bytes");
    auto p2b = get_mem_block(p2);
    assert(get_size(p2b) == 8);
    assert(get_next(p1b) == p2b);

    // --------------------------------------
    // Test case 3: Free the object
    //

    mem_free(p2);
    mem_dump("Operation 3: Free 8 bytes allocated above");
    assert(!is_used(p2b));

    // --------------------------------------
    // Test case 4: The block is reused
    //

    auto p3 = mem_alloc(8);
    mem_dump("Operation 4: Allocate 8 bytes, empty block is reused");
    auto p3b = get_mem_block(p3);
    assert(get_size(p3b) == 8);
    assert(p3b == p2b);

    // --------------------------------------
    // Test case 5: Blocks coalescing
    //

    auto p4 = mem_alloc(2);
    mem_dump("Operation 5: Allocate 2 bytes");
    auto p4b = get_mem_block(p4);
    assert(get_size(p4b) == 8);

    auto p5 = mem_alloc(4);
    mem_dump("Operation 6: Allocate 4 bytes");
    auto p5b = get_mem_block(p5);
    assert(get_size(p5b) == 8);

    mem_free(p5);
    mem_dump("Operation 7: Free 4 bytes allocated above");

    assert(get_next(p4b) != nullptr && !is_used(get_next(p4b)));

    mem_free(p4);
    mem_dump("Operation 8: Free 2 bytes allocated above, empty blocks are merged");

    assert(get_next(p4b) == nullptr);
    assert(get_size(p4b) == 16);

    //
    // --------------------------------------
    // Test case 6: Block splitting

    auto p6 = mem_alloc(4);
    mem_dump("Operation 9: Allocate 4 bytes, empty block is split");
    auto p6b = get_mem_block(p6);
    assert(p6b == p4b);
    assert(get_size(get_next(p6b)) == 8);
    assert(!is_used(get_next(p6b)));

    // --------------------------------------
    // Test case 7: Memory reallocation
    //

    // increase size when next block is empty
    assert(get_size(p6b) == 8);

    auto p7 = mem_realloc(p6, 13);
    mem_dump("Operation 10: Reallocate 8 bytes allocated above to 13 bytes");
    assert(p7 == p6);

    auto p7b = get_mem_block(p7);
    assert(get_size(p7b) == 16);
    assert(p7b == p6b);

    // decrease size
    auto p8 = mem_realloc(p7, 5);
    mem_dump("Operation 11: Reallocate 13 bytes allocated above to 5 bytes");
    assert(p8 == p7);

    auto p8b = get_mem_block(p8);
    assert(p8b == p7b);
    assert(get_size(p8b) == 8);
    assert(!is_used(get_next(p8b)));

    // increase size when next block is not exist
    auto p9 = mem_alloc(32);
    mem_dump("Operation 12: Allocate 32 bytes");

    auto p10 = mem_realloc(p9, 60);
    mem_dump("Operation 13: Reallocate 32 bytes to 60 bytes");
    assert(p9 == p10);

    auto p10b = get_mem_block(p10);
    assert(get_size(p10b) == 64);
    assert(is_used(p10b));
    assert(get_next(p10b) == nullptr);

    // increase size when next block is used
    mem_free(p10);
    mem_dump("Operation 14: Free last 12 bytes");

    auto p11 = mem_realloc(p1, 32);
    mem_dump("Operation 16: Reallocate 1 bytes to 32 bytes");

    auto p11b = get_mem_block(p11);
    assert(get_size(p11b) == 32);
    assert(is_used(p11b));

    puts("\nAll tests passed!\n");
}