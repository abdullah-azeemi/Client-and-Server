// huffman.h
#ifndef HUFFMAN_H
#define HUFFMAN_H

#define MAX_TREE_HT 100

// Huffman tree node
struct MinHeapNode {
    char data;              // One of the input characters
    unsigned freq;          // Frequency of the character
    struct MinHeapNode *left, *right; // Left and right child
};

// Min Heap: Collection of Min-Heap (or Huffman tree) nodes
struct MinHeap {
    unsigned size;          // Current size of min heap
    unsigned capacity;      // Capacity of min heap
    struct MinHeapNode **array; // Array of minheap node pointers
};

// Function prototypes
struct MinHeapNode *newNode(char data, unsigned freq);
struct MinHeap *createMinHeap(unsigned capacity);
void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b);
void minHeapify(struct MinHeap *minHeap, int idx);
int isSizeOne(struct MinHeap *minHeap);
struct MinHeapNode *extractMin(struct MinHeap *minHeap);
void insertMinHeap(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode);
void buildMinHeap(struct MinHeap *minHeap);
void printArr(int arr[], int n);
int isLeaf(struct MinHeapNode *root);
struct MinHeap *createAndBuildMinHeap(char data[], int freq[], int size);
struct MinHeapNode *buildHuffmanTree(char data[], int freq[], int size);
void printCodes(struct MinHeapNode *root, int arr[], int top);
void HuffmanCodes(char data[], int freq[], int size);

#endif // HUFFMAN_H
