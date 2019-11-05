#include <iostream>
#include "MemoryAllocator.h"

#define ALLOCATIONS_COUNT 10

using namespace std;

int randomNumber(int from, int to) {
    return int((random() % (to - from)) + from);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Error: Memory size is not indicated.\n";
        cout << "Put amount of memory in KB into program args!\n";
        return EXIT_SUCCESS;
    }
    int memorySize = stoi(argv[1]);

    srandom(time(nullptr));

    auto _ = MemoryAllocator(memorySize, Measure::K_BYTE);

    cout << "Memory initialized with next parameters\n";
    cout << "---------------------------------------\n";
    cout << "Memory size: " << _.getMeasuredSize() << " KB" << endl;
    cout << "Minimal block size: " << BLOCK_MIN_SIZE << " KB" << endl;
    cout << "Memory begins at address: " << (void *)(_.getMemoryPointer()) << endl;
    cout << "---------------------------------------\n\n";

    cout << "Initial memory state: " << endl;
    _.dump();
    cout << endl;

    Block *blocks[ALLOCATIONS_COUNT];

    for (auto &block : blocks) {
        int size = randomNumber(64, 256);
        cout << "Allocation of " << size << " KB" << endl;
        block = _.alloc(size);
        cout << "Address of allocation result: " << block->startAddress << endl;
        cout << "Free memory structure: " << endl;
        _.dump();
        cout << endl;
    }

    for (int i = ALLOCATIONS_COUNT - 1 ; i >= 0; i--) {
        cout << "Free of " << blocks[i]->size << " KB" << endl;
        _.free(blocks[i]);
        cout << "Free memory structure: " << endl;
        _.dump();
        cout << endl;
    }

    return EXIT_SUCCESS;
}