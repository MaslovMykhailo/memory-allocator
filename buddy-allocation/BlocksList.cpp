#include "BlocksList.h"
#include <iostream>

BlocksList::BlocksList() {
    BlocksList::init(DEFAULT_BLOCK_SIZE);
}
BlocksList::BlocksList(int blockSize) {
    BlocksList::init(blockSize);
}

void BlocksList::init(int blockSize) {
    _blockSize = blockSize;
    _head = nullptr;
    _tail = nullptr;
    _length = 0;
}

bool BlocksList::isEmpty() {
    return _head == nullptr && _tail == nullptr;
}

int BlocksList::getBlockSize() {
    return _blockSize;
}

int BlocksList::getLength() {
    return int(_length);
}

void BlocksList::print() {
    auto *iterator = new ListBlock;
    iterator = _head;

    while (iterator != nullptr) {
        std::cout << iterator->address << " ";
        iterator = iterator->next;
    }
}

ListBlock *BlocksList::get(unsigned int position) {
    auto *current = new ListBlock;
    current = _head;
    if (position > 0) {
        for (int i = 0; i < position; i++) {
            current = current->next;
        }
    }
    return current;
}

void BlocksList::unshift(void *address) {
    auto *newBlock = new ListBlock;
    newBlock->address = address;
    newBlock->next = nullptr;
    if (_length == 0) {
        _head = newBlock;
        _tail = newBlock;
    } else {
        newBlock->next = _head;
        _head = newBlock;
    }
    _length++;
}

void BlocksList::push(void *address) {
    auto *newBlock = new ListBlock;
    newBlock->address = address;
    newBlock->next = nullptr;
    if (_length == 0) {
        _head = newBlock;
        _tail = newBlock;
    }
    else {
        ListBlock *secondLastBlock = get((_length == 1) ? 0 : _length - 2);
        secondLastBlock->next = newBlock;
        _tail = newBlock;
    }
    _length++;
}

void BlocksList::insert(unsigned int position, void *address) {
    if (_length == 0 || position == 0) {
        unshift(address);
    }
    else {
        auto *newBlock = new ListBlock;
        newBlock->address = address;
        auto *beforeBlock = new ListBlock;
        beforeBlock = get(position - 1);
        newBlock->next = beforeBlock->next;
        beforeBlock->next = newBlock;
        _length++;
    }
}

void BlocksList::remove(unsigned int position) {
    if (_length != 0 && position < _length) {
        auto *blockForRemove = new ListBlock;
        blockForRemove = get(position);
        if (_length == 1) {
            _head = nullptr;
            _tail = nullptr;
        } else {
            if (position == 0) {
                _head = blockForRemove->next;
            } else {
                auto *blockBefore = new ListBlock;
                blockBefore = get(position - 1);
                blockBefore->next = blockForRemove->next;
                if (position == _length - 1) {
                    _tail = blockBefore;
                }
            }
        }
        delete blockForRemove;
        _length--;
    }
}