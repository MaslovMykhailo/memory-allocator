#ifndef BUDDY_ALLOCATION_BLOCKSLIST_H
#define BUDDY_ALLOCATION_BLOCKSLIST_H

#define DEFAULT_BLOCK_SIZE 64

struct ListBlock {
    void *address;
    ListBlock *next;
};

class BlocksList {
    private:
        ListBlock *_head;
        ListBlock *_tail;
        int _blockSize;
        unsigned int _length;

        void init(int blockSize);

    public:
        BlocksList();
        BlocksList(int blockSize);
        bool isEmpty();
        int getBlockSize();
        int getLength();
        void print();
        ListBlock *get(unsigned int position);
        void unshift(void *address);
        void push(void *address);
        void insert(unsigned int position, void *address);
        void remove(unsigned int position);
};


#endif //BUDDY_ALLOCATION_BLOCKSLIST_H
