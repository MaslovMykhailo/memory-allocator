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

    assert(align(3) == 4);
    assert(align(8) == 8);
    assert(align(12) == 12);
    assert(align(16) == 16);
    assert(align(20) == 20);

    auto p1 = mem_alloc(1);
    auto p1b = get_mem_block(p1);
    assert(get_size(p1b) == sizeof(word_t));


    // --------------------------------------
    // Test case 2: Exact amount of aligned bytes
    //

    auto p2 = mem_alloc(8);
    auto p2b = get_mem_block(p2);
    assert(get_size(p2b) == 8);
    assert(get_next(p1b) == p2b);

    // --------------------------------------
    // Test case 3: Free the object
    //

    mem_free(p2);
    assert(!is_used(p2b));

    // --------------------------------------
    // Test case 4: The block is reused
    //

    auto p3 = mem_alloc(8);
    auto p3b = get_mem_block(p3);
    assert(get_size(p3b) == 8);
    assert(p3b == p2b);

    // --------------------------------------
    // Test case 5: Blocks coalescing
    //

    auto p4 = mem_alloc(2);
    auto p4b = get_mem_block(p4);
    assert(get_size(p4b) == 4);

    auto p5 = mem_alloc(4);
    auto p5b = get_mem_block(p5);
    assert(get_size(p5b) == 4);

    mem_free(p5);

    assert(get_next(p4b) != nullptr && !is_used(get_next(p4b)));

    mem_free(p4);

    assert(get_next(p4b) == nullptr);
    assert(get_size(p4b) == 8);

    // --------------------------------------
    // Test case 6: Block splitting
    //

    auto p6 = mem_alloc(4);
    auto p6b = get_mem_block(p6);
    assert(p6b == p4b);
    assert(get_size(get_next(p6b)) == 4);
    assert(!is_used(get_next(p6b)));

    // --------------------------------------
    // Test case 7: Memory reallocation
    //

    // increase size
    assert(get_size(p6b) == 4);

    auto p7 = mem_realloc(p6, 8);
    assert(p7 == p6);

    auto p7b = get_mem_block(p7);
    assert(get_size(p7b) == 8);
    assert(p7b == p6b);

    // decrease size
    auto p8 = mem_realloc(p7, 4);
    assert(p8 == p7);

    auto p8b = get_mem_block(p8);
    assert(p8b == p7b);
    assert(get_size(p8b) == 4);
    assert(!is_used(get_next(p8b)));


    int i = 1;
    auto block = heapStart;

    while (block != nullptr) {
        std::cout << "Index: " << i++ << "\n";
        std::cout << "Size: " << get_size(block) << "\n";
        std::cout << "Is used: " << is_used(block) << "\n";
        std::cout << "--------\n";
        block = get_next(block);
    }


    puts("\nAll tests passed!\n");
}