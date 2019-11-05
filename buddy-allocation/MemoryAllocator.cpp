#include "MemoryAllocator.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <iomanip>

MemoryAllocator::MemoryAllocator() {
    init(MEMORY_DEFAULT_SIZE_KB, Measure::K_BYTE);
}

MemoryAllocator::MemoryAllocator(int size, Measure measure) {
    init(size, measure);
}

void MemoryAllocator::init(int size, Measure measure) {
    _measure = measure;
    _size = MemoryAllocator::calcSize(size, measure);
    _listsCount = getMeasuredSize();

    _memory = (char *)malloc(_size);
    if (_memory == nullptr) {
        std::cerr << "Error: Out of memory\n";
        exit(EXIT_SUCCESS);
    }

    _blocks = new BlocksList[_listsCount];
    for (int i = 0; i < _listsCount; i++) {
        _blocks[i] = BlocksList(MemoryAllocator::calcBlockSize(i));
    }

    auto measuredSize = int(getMeasuredSize());
    _blocks[getListIndex(measuredSize)].push(_memory);
}

int MemoryAllocator::getListsCount() {
    return getListIndex(int(getMeasuredSize())) + 1;
}

int MemoryAllocator::getListIndex(int size) {
    return ceil(log2(size) - log2(BLOCK_MIN_SIZE));
}

unsigned long MemoryAllocator::getSize() {
    return _size;
}

unsigned long MemoryAllocator::getMeasuredSize() {
    return _size / _measure;
}

void MemoryAllocator::sortList(int listIndex) {
    BlocksList unsorted = _blocks[listIndex];
    BlocksList sorted = BlocksList(unsorted.getBlockSize());
    sorted.unshift(unsorted.get(0)->address);

    for (int i = 1; i < unsorted.getLength(); i++) {
        void *insertAddress = unsorted.get(i)->address;
        for (int j = 0; j < sorted.getLength(); j++) {
            if (insertAddress <= (sorted.get(j)->address)) {
                sorted.unshift(insertAddress);
                break;
            } else if (
                j == sorted.getLength() - 1 &&
                insertAddress > (sorted.get(j)->address)
            ) {
                sorted.push(insertAddress);
                break;
            }
        }
    }

    _blocks[listIndex] = sorted;
}

int *MemoryAllocator::findBuddies(int listIndex) {
    BlocksList list = _blocks[listIndex];
    if (list.getLength() > 1) {
        int *buddies = (int *)malloc(sizeof(int) * 2);
        int blockSize = list.getBlockSize() * _measure;
        for (int i = 0; i <= list.getLength() - 2; i++) {
            if (list.get(i)->address == (char *) list.get(i + 1)->address - blockSize) {
                buddies[0] = i;
                buddies[1] = i + 1;
                return buddies;
            }
        }
    }
    return nullptr;
}

void MemoryAllocator::merge(int listIndex) {
    int *buddies = findBuddies(listIndex);
    while (buddies != nullptr) {
        void* startAddress = _blocks[listIndex].get(buddies[0])->address;
        _blocks[listIndex].remove(buddies[1]);
        _blocks[listIndex].remove(buddies[0]);
        _blocks[listIndex + 1].unshift(startAddress);
        sortList(listIndex + 1);

        buddies = findBuddies(listIndex);
    }
}

void MemoryAllocator::mergeAll() {
    for (int i = 0; i < int(_listsCount); i++) {
        merge(i);
    }
}

void MemoryAllocator::dump() {
    for (int i = 0; i < getListsCount(); i++) {
        std::cout << std::internal << std::setw(8);
        std::cout << _blocks[i].getBlockSize() << ": ";
        std::cout << "[ ";
        _blocks[i].print();
        std::cout << "]" << std::endl;
    }
}

char *MemoryAllocator::getMemoryPointer() {
    return _memory;
}

Block *MemoryAllocator::alloc(int size) {
    int listIndex = getListIndex(size);
    bool found = false;

    auto *allocatedBlock = new ListBlock;
    while (!found) {
        if (_blocks[listIndex].getLength() > 0) {
            allocatedBlock = _blocks[listIndex].get(0);
            _blocks[listIndex].remove(0);
            found = true;
        } else if (listIndex < _listsCount) {
            listIndex++;
            if (_blocks[listIndex].getLength() > 0) {
                auto *blockToSplit = new ListBlock;
                blockToSplit = _blocks[listIndex].get(0);
                _blocks[listIndex - 1].unshift((
                    (char *) blockToSplit->address + (_blocks[listIndex - 1].getBlockSize() * _measure)
                ));
                _blocks[listIndex - 1].unshift(blockToSplit->address);
                _blocks[listIndex].remove(0);
                listIndex = getListIndex(size);
            }
        } else {
            break;
        }
    }

    if (found) {
        auto *foundBlock = new Block;
        foundBlock->startAddress = (char *) allocatedBlock->address;
        foundBlock->size = _blocks[listIndex].getBlockSize();
        return foundBlock;
    } else {
        return nullptr;
    }
}

void MemoryAllocator::free(Block *freeBlock) {
    int listIndex = getListIndex(int(freeBlock->size));
    _blocks[listIndex].unshift(freeBlock->startAddress);
    sortList(listIndex);
    mergeAll();
}

unsigned long MemoryAllocator::calcSize(int size, Measure measure) {
    return size * measure * sizeof(char);
}

int MemoryAllocator::calcBlockSize(int index) {
    return pow(2, index + log2(BLOCK_MIN_SIZE));
}


