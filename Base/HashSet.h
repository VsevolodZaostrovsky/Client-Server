#pragma once
#include <iostream>
#include <list>
#include <iterator>
#include "Storage.h"

//Реализуется хэш-множество. В частности, массив мощности HASH_SET_LEN, за каждой позицией которого закреплён список.

struct Couple
{
    std::string Name;
    int ind;

    Couple(std::string n, int i) : Name(n), ind(i) {}


    //Мощность ОДЗ хэш-функции
    #define HASH_SET_LEN 10
    //Собственно, хэш-функция
    int hashFunc() const
    {
        int sum = 0;
        for (int i = 0; i < Name.size(); ++i)
        {
            sum += (int)Name[i];
        }
        return sum % HASH_SET_LEN;
    }

    bool operator==(const Couple &C) const {
        if(Name == C.Name) { return 1; }
        return 0;
    }

    friend std::ostream & operator<<(std::ostream & os, const Couple & C) {

        std::cout << C.Name << " = " << C.ind;
        return os;
    }   
    
};
//Мощность её ОДЗ
#define HASH_SET_LEN 10

typedef std::list<Couple> CoupleList;

struct HashSet
{
    CoupleList *Array;

    HashSet()
    {
        Array = new CoupleList[HASH_SET_LEN];
    }

    ~HashSet()
    {
        delete[] Array;
    }

    bool add(std::string Name, int index)
    {
        Couple C(Name, index);
        int ind = C.hashFunc();
        Array[ind].remove(C);
        Array[ind].emplace_back(C);
        return 1;
    }

    bool del(std::string Name) {
        Couple C(Name, 0);
        int ind = C.hashFunc();
        Array[ind].remove(C);
        return 1;
    }

    //Возвращает index Name, если элемент найден, или -1 иначе
    int find(std::string Name) {
        Couple C(Name, 0);
        int ind = C.hashFunc();
        for (auto iter = Array[ind].begin(); iter != Array[ind].end(); iter++) {
        if (*iter == C) { return iter->ind; }
        }
        return -1;
    }

    int print()
    {
        for(int i = 0; i < HASH_SET_LEN; ++i) {
            std::cout<<"position " << i << std::endl;
            copy(Array[i].begin(), Array[i].end(), std::ostream_iterator<Couple>(std::cout, " "));
            std::cout<<std::endl;
        }
        return 1;
    }
};