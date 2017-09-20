/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: prashant
 *
 * Created on March 26, 2017, 6:48 PM
 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <bitset>

#define FREQ_TAB_SIZE   10000000

using namespace std;

/* Huffman tree structure */
struct huffTree {
    int dFrequency;
    int data;
    struct huffTree *left;
    struct huffTree *right;
};

/* Pairing heap node structure */
struct pairingHeapNode {
    struct huffTree *huffData;
    struct pairingHeapNode *child;
    struct pairingHeapNode *leftSibling;
    struct pairingHeapNode *rightSibling;
};


/************  Binary heap  ***************/
class BinaryHeap {
public:
    vector <struct huffTree *> heap;
    struct huffTree *root;
    int heapSize;
    BinaryHeap();
    struct huffTree *buildTreeUsingBinaryHeap(unordered_map <int, int>);

private:
    void buildHeap();
    struct huffTree *buildHuffTree();
    void binHeapInsert(struct huffTree *);
    struct huffTree *extractMin();
    void minHeapify(int);
    int parent(int);
    int left(int);
    int right(int);
};


/**************** Cache optimized 4 way heap ********** */
class FourWayHeap {
public:
    struct huffTree *root;
    unordered_map<int, string> codeTable;
    FourWayHeap();
    struct huffTree *buildTreeUsing4wayHeap(unordered_map<int, int>);
    void buildCodeTable (struct huffTree *, vector<char> );

private:
    int heapSize;
    vector<struct huffTree *> heap4way;
    void build4way();
    void insert4way(struct huffTree *);
    void heapify(int);
    struct huffTree *extractMin();
    struct huffTree *buildHuffTree();
    int parent(int);
};


/****************  Pairing heap  ****************/
class PairingHeap {
public:
    struct huffTree *root;
    PairingHeap();
    struct huffTree *buildTreeUsingPairingHeap(unordered_map<int, int>);

private:
    struct pairingHeapNode *heapRoot;
    void meld(struct pairingHeapNode * &tree1, struct pairingHeapNode *tree2);
    struct pairingHeapNode *twoPassMeld(struct pairingHeapNode *);
    void buildPairingHeap(unordered_map <int, int>);
    struct pairingHeapNode *deleteMin();
    struct pairingHeapNode *Insert(struct huffTree *);
    struct huffTree *buildHuffTree();
};


/*************   Encoder class declaration ***************/
class Encoder {

private:
    unordered_map <int, string> codeTable;
    struct huffTree *huffRoot;
    string inputFile;
    
public:
    Encoder(struct huffTree *, string, unordered_map<int, string>);
    void encodeData ();
    void codeTableOutput (unordered_map <int, string>);
    
};


/*
 *
 * Binary Heap functions definition
 * 
 */

BinaryHeap::BinaryHeap() {
    heapSize = 0;
    root = NULL;
}

struct huffTree *BinaryHeap::buildTreeUsingBinaryHeap(unordered_map<int, int> fmap) {

    heap.resize(fmap.size());
    for (auto it : fmap) {
        struct huffTree * temp = new huffTree;
        temp->dFrequency = it.second;
        temp->data = it.first;
        temp->left = NULL;
        temp->right = NULL;
        heap[heapSize++] = temp;
    }

    buildHeap();


    return buildHuffTree();
}

int BinaryHeap::parent(int index) {
    return index / 2;
}

int BinaryHeap::left(int index) {
    return index * 2;
}

int BinaryHeap::right(int index) {
    return (index * 2 + 1);
}

void BinaryHeap::minHeapify(int index) {
    int leftChild = left(index);
    int rightChild = right(index);
    int smallest;

    if (leftChild < heapSize &&
            heap[leftChild]->dFrequency < heap[index]->dFrequency)
        smallest = leftChild;
    else
        smallest = index;

    if (rightChild < heapSize &&
            heap[rightChild]->dFrequency < heap[smallest]->dFrequency)
        smallest = rightChild;

    if (smallest != index) {
        struct huffTree *temp = heap[index];
        heap[index] = heap [smallest];
        heap[smallest] = temp;
        minHeapify(smallest);
    }
}

void BinaryHeap::buildHeap() {
    for (int i = heapSize / 2; i >= 0; i--)
        minHeapify(i);
}

struct huffTree *BinaryHeap::extractMin() {
    if (heapSize == 0)
        return NULL;

    struct huffTree *min = heap[0];
    heap[0] = heap[heapSize - 1];
    heapSize--;
    minHeapify(0);
    return min;
}

void BinaryHeap::binHeapInsert(struct huffTree *subTree) {

    int insertIndex = heapSize++;
    heap[insertIndex] = subTree;

    int parentIndex = parent(insertIndex);
    while (insertIndex >= 0 &&
            (heap[insertIndex]->dFrequency < heap[parentIndex]->dFrequency)) {

        struct huffTree *temp = heap[parentIndex];
        heap[parentIndex] = heap[insertIndex];
        heap[insertIndex] = temp;
        insertIndex = parentIndex;
        parentIndex = parent(insertIndex);
    }

}

struct huffTree *BinaryHeap::buildHuffTree() {

    while (heapSize != 1) {
        struct huffTree *left = extractMin();
        struct huffTree *right = extractMin();

        /* new tree that has to be inserted in the heap by combining
         * above two trees
         */
        struct huffTree *newTree = new huffTree;

        newTree->left = left;
        newTree->right = right;
        newTree->dFrequency = left->dFrequency + right->dFrequency;

        /* Assigning newTree->data to -1 */
        newTree->data = -1;

        binHeapInsert(newTree);
    }

    root = heap[heapSize - 1];
    return root;
}

/*
 *
 * Four way heap functions definition
 *  
 */

FourWayHeap::FourWayHeap() {
    heapSize = 0;
    root = NULL;
}

struct huffTree *FourWayHeap::buildTreeUsing4wayHeap(unordered_map<int, int> fmap) {
    heap4way.resize(fmap.size() + 3);
    for (auto it : fmap) {
        struct huffTree *temp = new huffTree;
        temp->data = it.first;
        temp->dFrequency = it.second;
        temp->left = NULL;
        temp->right = NULL;
        heap4way[heapSize + 3] = temp;
        heapSize++;
    }

    build4way();

    return buildHuffTree();
}

int FourWayHeap::parent(int index) {
    return ((index - 4) / 4) + 3;
}

void FourWayHeap::heapify(int index) {
    if (heapSize == 0)
        return;

    int smallest;

    int children1 = ((index - 3)*4 + 4);
    if (children1 < heapSize + 3 &&
            heap4way[children1]->dFrequency < heap4way[index]->dFrequency)
        smallest = children1;
    else
        smallest = index;

    int children2 = ((index - 3)*4 + 5);
    if (children2 < heapSize + 3 &&
            heap4way[children2]->dFrequency < heap4way[smallest]->dFrequency)
        smallest = children2;

    int children3 = ((index - 3)*4 + 6);
    if (children3 < heapSize + 3 &&
            heap4way[children3]->dFrequency < heap4way[smallest]->dFrequency)
        smallest = children3;

    int children4 = ((index - 3)*4 + 7);
    if (children4 < heapSize + 3 &&
            heap4way[children4]->dFrequency < heap4way[smallest]->dFrequency)
        smallest = children4;

    if (smallest != index) {
        struct huffTree *temp = heap4way[index];
        heap4way[index] = heap4way[smallest];
        heap4way[smallest] = temp;

        heapify(smallest);
    }
}

void FourWayHeap::build4way() {
    for (int index = ((heapSize - 1) / 4) + 3; index >= 3; index--) {
        heapify(index);
    }
}

void FourWayHeap::insert4way(struct huffTree *tree) {

    heap4way[heapSize + 3] = tree;
    int parentIndex = parent(heapSize + 3);
    int index = heapSize + 3;
    heapSize++;

    while (index >= 3 &&
            (heap4way [index]->dFrequency < heap4way [parentIndex]->dFrequency)) {
        struct huffTree *temp = heap4way[parentIndex];
        heap4way[parentIndex] = heap4way[index];
        heap4way[index] = temp;
        index = parentIndex;
        parentIndex = parent(index);
    }
}

struct huffTree *FourWayHeap::extractMin() {
    struct huffTree *min = heap4way[3];

    heap4way[3] = heap4way[heapSize + 2];
    heapSize--;
    heapify(3);

    return min;
}

struct huffTree *FourWayHeap::buildHuffTree() {

    while (heapSize > 1) {
        struct huffTree *left = extractMin();
        struct huffTree *right = extractMin();

        /* new tree that has to be inserted in the heap by combining
         * above two trees
         */
        struct huffTree *newTree = new huffTree;

        newTree->left = left;
        newTree->right = right;
        newTree->dFrequency = left->dFrequency + right->dFrequency;

        /* Assigning newTree->data to -1 */
        newTree->data = -1;

        insert4way(newTree);
        
    }

    root = heap4way[3];
    return heap4way[3];
}

void FourWayHeap::buildCodeTable(struct huffTree *node, vector<char> codeBuilder) {

    if (node == NULL)
        return;
    
    codeBuilder.push_back('0');
    buildCodeTable(node->left, codeBuilder);
    
    codeBuilder.pop_back();
    if (node->data != -1) {
        codeTable[node->data] = string(codeBuilder.data(), codeBuilder.size());
    }
 
    codeBuilder.push_back('1');
    buildCodeTable(node->right, codeBuilder);
}

/*
 *
 *  Pairing heap methods definition
 * 
 */

PairingHeap::PairingHeap() {
    heapRoot = NULL;
    root = NULL;
}

struct huffTree *PairingHeap::buildTreeUsingPairingHeap(unordered_map <int, int> ftab) {

    buildPairingHeap(ftab);

    return buildHuffTree();
}

struct pairingHeapNode *PairingHeap::Insert(struct huffTree *node) {

    struct pairingHeapNode *newNode = new pairingHeapNode;
    newNode->huffData = node;
    newNode->child = NULL;
    newNode->leftSibling = NULL;
    newNode->rightSibling = NULL;

    if (heapRoot == NULL)
        heapRoot = newNode;
    else
        meld(heapRoot, newNode);

    return newNode;
}

struct pairingHeapNode *PairingHeap::deleteMin() {

    struct pairingHeapNode *oldRoot = heapRoot;

    if (heapRoot) {
        if (heapRoot->child == NULL)
            heapRoot = NULL;
        else
            heapRoot = twoPassMeld(heapRoot->child);
    }

    return oldRoot;
}

void PairingHeap::meld(struct pairingHeapNode *&tree1,
        struct pairingHeapNode *tree2) {

    if (tree2 == NULL)
        return;

    if (tree2->huffData->dFrequency < tree1->huffData->dFrequency) {
        tree2->leftSibling = tree1->leftSibling;
        tree1->leftSibling = tree2;
        tree1->rightSibling = tree2->child;

        if (tree1->rightSibling != NULL)
            tree1->rightSibling->leftSibling = tree1;

        tree2->child = tree1;
        tree1 = tree2;
    } else {
        tree2->leftSibling = tree1;
        tree1->rightSibling = tree2->rightSibling;

        if (tree1->rightSibling != NULL)
            tree1->rightSibling->leftSibling = tree1;

        tree2->rightSibling = tree1->child;

        if (tree2->rightSibling != NULL)
            tree2->rightSibling->leftSibling = tree2;
        tree1->child = tree2;
    }
}

struct pairingHeapNode *PairingHeap::twoPassMeld(struct pairingHeapNode *firstSibling) {

    if (firstSibling->rightSibling == NULL)
        return firstSibling;

    static vector<struct pairingHeapNode *> treeArray(5);

    int numSiblings = 0;

    for (; firstSibling != NULL; numSiblings++) {

        if (numSiblings == treeArray.size())
            treeArray.resize(numSiblings * 2);

        treeArray[numSiblings] = firstSibling;
        firstSibling->leftSibling->rightSibling = NULL;
        firstSibling = firstSibling->rightSibling;
    }

    if (numSiblings == treeArray.size())
        treeArray.resize(numSiblings + 1);

    treeArray[numSiblings] = NULL;

    int i = 0;
    for (; i + 1 < numSiblings; i += 2)
        meld(treeArray[i], treeArray[i + 1]);

    int j = i - 2;

    if (j == numSiblings - 3)
        meld(treeArray[j], treeArray[j + 2]);

    for (; j >= 2; j -= 2)
        meld(treeArray[j - 2], treeArray[j]);

    return treeArray[0];

}

void PairingHeap::buildPairingHeap(unordered_map <int, int> fTable) {

    for (auto it : fTable) {

        struct huffTree *huffNode = new huffTree;
        huffNode->dFrequency = it.second;
        huffNode->data = it.first;
        huffNode->left = NULL;
        huffNode->right = NULL;

        Insert(huffNode);
    }
}

struct huffTree *PairingHeap::buildHuffTree() {

    while (1) {
        struct pairingHeapNode * tree1 = deleteMin();
        if (tree1 == NULL)
            return NULL;
        struct pairingHeapNode *tree2 = deleteMin();
        if (tree2 == NULL) {
            root = tree1->huffData;
            return tree1->huffData;
        }

        struct huffTree * temp = new huffTree;
        temp->data = -1;
        temp->dFrequency = tree1->huffData->dFrequency + tree2->huffData->dFrequency;
        temp->left = tree1->huffData;
        temp->right = tree2->huffData;

        delete tree1;
        delete tree2;

        Insert(temp);
    }
}

/*
 *
 * Encoder function definitions
 * 
 */

Encoder::Encoder(struct huffTree *root, string ifile,
        unordered_map<int, string> codeTab) {
    huffRoot = root;
    inputFile = ifile;
    codeTable = codeTab;
}

void Encoder::encodeData() {

    ifstream ifile;
    ofstream encodedFile;

    ifile.open(inputFile);

    if (!ifile.is_open()) {
        cout << "Encoding failed! Input file failed to open" << endl;
        return;
    }

    encodedFile.open("encoded.bin", ios::binary);

    if (!encodedFile.is_open()) {
        cout << "Encoding failed! Output file could not be loaded" << endl;
        return;
    }

    string bits;
    unsigned char oneByte;
    int number;
    int i = 0;

    while (ifile >> number) {
        for (char ch : codeTable[number]) {
            bits[i++] = ch;

            if (i == 8) {
                oneByte = (unsigned char) (std::strtoul(bits.c_str(), 0, 2) & 0xff);
                encodedFile << oneByte;

                i = 0;
            }
        }
    }
    
    encodedFile.close();
}

void Encoder::codeTableOutput (unordered_map <int, string> ctable){
    ofstream codeTableFile;
    codeTableFile.open ("code_table.txt");
    
    for (auto codeEntry : ctable){
        codeTableFile << codeEntry.first << " " << codeEntry.second << endl;
    }
    codeTableFile.close();
}


int main(int argc, char *argv []) {

    if (argc != 2){
        cout << "Error! Enter input file name" << endl;
        return -1;
    }
    
    int number;
    unordered_map<int, int> frequencyTab;

    string inputFile = argv[1];
    ifstream ifile;
    ifile.open(inputFile);

    if (!ifile.is_open()) {
        cout << "Error!!! File did not open!" << endl;
        return -1;
    }
    
    clock_t startTime;
    startTime = clock();
    
    while (ifile >> number) {
        if (frequencyTab.find(number) != frequencyTab.end())
            frequencyTab[number] += 1;
        else
            frequencyTab[number] = 1;
    }

    // 4-way heap
    FourWayHeap fwh;
    
    fwh.buildTreeUsing4wayHeap(frequencyTab);

    vector<char> codeHolder;
    fwh.buildCodeTable(fwh.root, codeHolder);
    
    Encoder encoder(fwh.root, inputFile, fwh.codeTable);
    encoder.encodeData();
    encoder.codeTableOutput(fwh.codeTable);
    
    cout << "Total time in encoding (microseconds): " << ((clock() - startTime)) << endl;

    return 0;
}

