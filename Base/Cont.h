#pragma once
#include "Storage.h"
#define BLOCK_SIZE 2


//Реализуется структура данных:
//Объекты хранятся в памяти как список массивов
//При удалении элемента другие элементы не сдвигаются. При добавлении, новый элемент помещается на первое свободное место

class Cont
{
private:
    struct Block
    {
        int FreePlaces = BLOCK_SIZE;
        Item *Array = nullptr;
        Block *Next = nullptr;
        Block *Prev = nullptr;
        bool *IsFree = nullptr; //массив показывает, свободна ли i-ая позиция блока

        Block() {
            Array = new Item[BLOCK_SIZE];
            IsFree = new bool[BLOCK_SIZE];
            for (int i = 0; i < BLOCK_SIZE; ++i)
            {
                IsFree[i] = 1;
            }
        }

        ~Block() {
            FreePlaces = 0;
            if(IsFree) { delete [] IsFree; }
            if(Array) { delete [] Array; }
            Next = nullptr;
            Prev = nullptr;
        }
    
        int print() {
            for(int i = 0; i < BLOCK_SIZE; ++i) {
                if(IsFree[i] == 0) { Array[i].print(); }
            }
            return 1;
        }
    };

    int BlockNum = 0;
    int NumOfObj = 0;
    Block *Head = nullptr;
    Block *Tail = nullptr;

public:
    Cont()
    {
        Head = new Block;
        Tail = Head;
        BlockNum = 1;
    }

    ~Cont()
    {
//        printf("~Cont start\n");
        Block *curr = Head;
        Block *next = nullptr;
        while (curr)
        {
            next = curr->Next;
            delete curr;
            curr = next;
        }
        Head = nullptr;
        Tail = nullptr;
        NumOfObj = 0;
        BlockNum = 0;
//        printf("Cont destructed\n");
    } 

    Item &operator[](const int i) const {
        if(i > BlockNum*BLOCK_SIZE || i < 0) { /*error */ }
        int p = (int)(i/BLOCK_SIZE);
        Block* curr = Head;
        while(p) {
            curr = curr->Next;
            p--;
        }
        if(curr->IsFree[i%BLOCK_SIZE] == 1) {
            Item* ans = new Item[1];
            ans->CompLen = -1;
            ans->Name = "Empty";
            return (*ans);
        }
        return curr->Array[i%BLOCK_SIZE];
    }

    bool isFree(const int i) const {
        if(i > BlockNum*BLOCK_SIZE || i < 0) { return 0; }
        int p = (int)(i/BLOCK_SIZE);
        Block* curr = Head;
        while(p) { curr = curr->Next; p--; }
        return curr->IsFree[i%BLOCK_SIZE];
    }

    bool change(int i, int quant, int complen, int* compids, int* compqts) {
        if(i > BlockNum*BLOCK_SIZE-1 || i < 0 || quant < 0 || complen < 0) { return 0; }
        int p = (int)(i/BLOCK_SIZE);
        Block* curr = Head;
        while(p) { curr = curr->Next; p--; }
        if(curr->IsFree[i%BLOCK_SIZE] == 1) { return 0; }
        curr->Array[i%BLOCK_SIZE].Quant = quant;
        if(curr->Array[i%BLOCK_SIZE].CompLen > 0) {
            delete [] curr->Array[i%BLOCK_SIZE].CompIndexes;
            delete [] curr->Array[i%BLOCK_SIZE].CompQuants;
        }
        curr->Array[i%BLOCK_SIZE].CompLen = complen;
        if(complen == 0) { return 1; }
        if(complen > 0) {
            curr->Array[i%BLOCK_SIZE].CompIndexes = new int[complen];
            curr->Array[i%BLOCK_SIZE].CompQuants = new int[complen];
            for (int j = 0; j < complen; ++j) {
                curr->Array[i%BLOCK_SIZE].CompIndexes[j] = compids[j];
                curr->Array[i%BLOCK_SIZE].CompQuants[j] = compqts[j];
            }
            return 1;
        }
        return 0;
    }

    bool changeOnlyQuant(int i, int quant) {
        if(quant < 0) { return 0; }
        (*this)[i].Quant = quant;
        return 1;
    }

    int add(Item &C) {
        Block* curr = Head;
        int ind = 0;
        while(curr) {
            if(curr->FreePlaces) {
                curr->FreePlaces--;
                for(int i = 0; i < BLOCK_SIZE; ++i) {
                    if(curr->IsFree[i]) { 
                        curr->Array[i] = C;
                        curr->IsFree[i] = 0;
                        curr->Array[i].Index = ind + i;
                        NumOfObj++;
                        return (ind + i); 
                    }              
                }
            }
            ind += BLOCK_SIZE;
            curr = curr->Next;
        }
        Tail->Next = new Block;
        Tail = Tail->Next;
        BlockNum++;
        Tail->Array[0] = C; Tail->IsFree[0] = 0;
        Tail->Array[0].Index = ind;
        NumOfObj++;
        return ind;
    }

    int del(int i) {
        if(i > BlockNum*BLOCK_SIZE || i < 0) { return 0; }
        int p = (int)(i/BLOCK_SIZE);
        Block* curr = Head;
        while(p) {
            curr = curr->Next;
            p--;
        }
//        delete (curr->Array[i%BLOCK_SIZE]);
        curr->IsFree[i%BLOCK_SIZE] = 1;
        curr->FreePlaces += 1;
        return 1;

    }

    int getNumOfObj() const { return NumOfObj; }

    int size() const { return BlockNum*BLOCK_SIZE; }

    int print() {
        std::cout << "Cont: \n";
        Block* curr = Head;
        while(curr) {
            curr->print();
            curr = curr->Next;
        }
        return 1;
    }
};