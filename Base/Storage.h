#pragma once
#include <iostream>
#include <fstream>
#include <string>

#define THERE_IS_NOT_THAT_DETAIL -2
#define DETAIL_CANT_BE_CREATED -1

//typedef int index; //Здесь и далее: индекс детали в массиве AllItems. Он не меняется, даже если предыдущую деталь удалили.
typedef enum { LIST_OK = 0, INVALID_REQUEST = 1 } ItemListErrorType;

int min(int* array, int len);
int compareInd(int* A, int lenA, int* B, int lenB);
int compareStr(std::string* A, int lenA, std::string* B, int lenB);
int areThereSimilars(int* A, int lenA);
int areThereSimilarsStr(std::string* A, int lenA);
int isThere(int* A, int lenA, int X);
bool areThereNegaivesOrZero(int* A, int lenA);


struct Item
{
    int Index;
    std::string Name;
    int Quant = 0;
    
    //сборочная карта
    int CompLen = 0;  //если это число меньше нуля, то объект возник при обрвщении к несуществующей детали
    int* CompIndexes; //здесь лежат именно индексы деталей (если что, имена восстанавливаются быстро)
    int* CompQuants;

    Item() = default;

    Item(const Item &I) : Index(I.Index), Name(I.Name), Quant(I.Quant), CompLen(I.CompLen)
    {
        if (CompLen > 0)
        {
            CompIndexes = new int[CompLen]; 
            //(index *)malloc(CompLen * sizeof(index));
            CompQuants = new int[CompLen];
            //(int *)malloc(CompLen * sizeof(int));
            for (int i = 0; i < CompLen; ++i)
            {
                CompIndexes[i] = I.CompIndexes[i];
                CompQuants[i] = I.CompIndexes[i];
            }
        }
    }

    Item(int i, std::string n, int q) : Index(i), Name(n), Quant(q) {}
    
    Item(int i, std::string n, int q, int len, int* ci, int* cq) 
    : Index(i), Name(n), Quant(q), CompLen(len) {
        CompIndexes = new int[CompLen];
        CompQuants = new int[CompLen];
        for(int i = 0; i < CompLen; ++i) {
            CompIndexes[i] = ci[i];
            CompQuants[i] = cq[i];
        }
    }

    ~Item() {
//        printf("~Item start\n");
        if(CompLen) {
            delete [] CompIndexes;
            delete [] CompQuants;
        }
//        printf("Item destructed\n");
    }

    int print() const {
        std::cout << "Name: " << Name << std::endl;
        std::cout << "ID: " << Index << std::endl; 
        std::cout << "Quant: " << Quant << std::endl;
        if (CompLen) {
            for (int i = 0; i < CompLen; ++i) {
                std::cout << CompIndexes[i] << ": " << CompQuants[i] << std::endl;
            }
        }
//        else std::cout << "Impossible to create" << std::endl;
        return 1;
    }

    Item& operator=(const Item &I) {
        if (this == &I) {
            return *this;
        }
        Index = I.Index; 
        Name = I.Name; 
        Quant = I.Quant;
        if(CompLen > 0) {
            delete [] CompIndexes;
            delete [] CompQuants;
            CompLen = 0;
        }
        if(I.CompLen > 0) {
            CompLen = I.CompLen;
            CompQuants = new int[CompLen];
            CompIndexes = new int[CompLen];
            for(int i = 0; i < CompLen; i++) {
                CompIndexes[i] = I.CompIndexes[i];
                CompQuants[i] = I.CompQuants[i];
            }
        }
        return *this;
}
};

struct ItemList {
    ItemListErrorType error;
    int* HowManyNeed;
    std::string* Names;
    int Len = 0;
    std::string Name = " ";
    int Quant = -1;

    ~ItemList() {
        if(Len > 0) {
            delete [] HowManyNeed;
            delete [] Names;
        }
    }

    void print() {
        if(Quant >= 0) {
            std::cout << "Name: " << Name << std::endl;
            std::cout << "Quant: " << Quant << std::endl;
        }
        if(Len == 0 && Quant == -1) { std::cout << "empty list\n"; }
        for(int i = 0; i < Len; ++i) {
            std::cout << HowManyNeed[i] << " " << Names[i] << " needed" << std::endl;
        }
    }
};

#include "HashSet.h"
#include "Cont.h"

class Storage {
    private:
        Cont* AllItems;  //Здесь просто лежат все детали
        HashSet* NameIndex;   //Эта структура позволяет получить из имени детали её индекс в AllItems. 

    public:
        Storage() {
            AllItems = new Cont;
            NameIndex = new HashSet;
        }

        Storage(std::string filename) {
            AllItems = new Cont;
            NameIndex = new HashSet;
            readFrom(filename);
        }
        
        ~Storage() {
            delete AllItems;
            delete NameIndex;
        }

        bool readFrom(std::string filename)
        {
            std::ifstream inp(filename);
            int p = inp.is_open();
            if (!p)
            {
                inp.close();
                return false;
            }
            while (!inp.eof())
            {
                std::string KeyWord;
                std::string Name;
                int Quant;
                int CompLen;
                inp >> KeyWord >> Name >> KeyWord >> Quant >> KeyWord >> CompLen;
                if(Name == "") { inp.close(); return 1;}
                if (CompLen > 0)
                {
                    std::string *cn = new std::string[CompLen];
                    int *pq = new int[CompLen];
                    for (int k = 0; k < CompLen; ++k)
                    {
                        inp >> cn[k] >> pq[k];
//                        std::cout << cn[k] << ":" << pq[k] << std::endl;
                    }
//                    std::cout << "Read:" << Name << " " << Quant << " " << CompLen << std::endl;
                    PushItem(Name, Quant, CompLen, cn, pq);
                    delete[] cn;
                    delete[] pq;
                }
                if (CompLen == 0)
                {
//                    std::cout << "Read:" << Name << " " << Quant << " " << CompLen << std::endl;
                    PushItem(Name, Quant, CompLen, nullptr, nullptr);
                }
            }
            inp.close();
            return true;
        }

        bool writeTo(std::string filename) const
        {
            std::ofstream out;
            out.open(filename);
            if (out.is_open())
            {
                for (int i = 0; i < AllItems->size(); ++i)
                {
                    if (!AllItems->isFree(i))
                    {
                        out << "Name: " << (*AllItems)[i].Name << std::endl;
                        out << "Quant: " << (*AllItems)[i].Quant << std::endl;
                        out << "CompLen: " << (*AllItems)[i].CompLen << std::endl;
                        if ((*AllItems)[i].CompLen)
                        {
                            for (int j = 0; j < (*AllItems)[i].CompLen; ++j)
                            {
                                int t = (*AllItems)[i].CompIndexes[j];
                                out << (*AllItems)[t].Name << " " << (*AllItems)[i].CompQuants[j] << std::endl;
                            }
                        }
                    }
                }
                return true;
            }
            else return false;
        }

        int howManyCanCreate(std::string name) const {
            int id = NameIndex->find(name);
            if(id < 0) { return THERE_IS_NOT_THAT_DETAIL; }
            const Item &I = (*AllItems)[id];
            int HowManyLen = I.CompLen;
            if(HowManyLen <= 0) { return DETAIL_CANT_BE_CREATED; }
            int* HowManyArray = new int[HowManyLen];
            for(int h = 0; h < HowManyLen; ++h) {
                HowManyArray[h] = (((*AllItems)[I.CompIndexes[h]]).Quant) / (I.CompQuants[h]);
            }
            int ans = min(HowManyArray, HowManyLen);
            delete [] HowManyArray;
            return ans;
        }

        ItemList* whatWeNeedToCreate(std::string name, int quant) const { //Возвращает описание деталей, которых не хватает для создания quant устройств имени name
            int id = NameIndex->find(name);
            if(id < 0) { return nullptr; }
            if(howManyCanCreate(name) >= quant) { return nullptr; }
            const Item &I = (*AllItems)[id];
            int HowManyLen = I.CompLen;
            int* HowManyArray = new int[HowManyLen];
            int NumOfDeficitDetails = 0;
            for(int h = 0; h < HowManyLen; ++h) {
                int place = (I.CompQuants[h]) * quant - (((*AllItems)[I.CompIndexes[h]]).Quant);
                if(place > 0) { HowManyArray[h] = place; NumOfDeficitDetails += 1; }
                if(place <= 0) { HowManyArray[h] = 0; }
            }
            ItemList* ans = new ItemList[1];
            ans->Len = NumOfDeficitDetails;
            ans->error = LIST_OK;
            int j = 0;
            ans->Names = new std::string[NumOfDeficitDetails];
            ans->HowManyNeed = new int[NumOfDeficitDetails];
            for(int k = 0; k < HowManyLen; ++k) {
                if(HowManyArray[k]) { 
                    ans->Names[j] = (*AllItems)[I.CompIndexes[k]].Name;
                    ans->HowManyNeed[j] = HowManyArray[k];
                    j++;
                }
            }
            delete [] HowManyArray;
            return ans;
        }

        ItemList* deficitAfterCreate(std::string name, int quant) const { //Возвращает описание деталей, которых будет не хватать для создания 1го устройства имени name, после создания quant таких деталей
            int id = NameIndex->find(name);
            if(id < 0) { return nullptr; }
            if(howManyCanCreate(name) != quant) { return nullptr; }
            const Item &I = (*AllItems)[id];
            int DeficitLen = I.CompLen;
            int* DeficitArray = new int[DeficitLen];
            int NumOfDeficitDetails = 0;
            for(int h = 0; h < DeficitLen; ++h) {
                int place = (I.CompQuants[h]) * (quant + 1) - (((*AllItems)[I.CompIndexes[h]]).Quant);
                if(place > 0) { DeficitArray[h] = place; NumOfDeficitDetails += 1; }
                if(place <= 0) { DeficitArray[h] = 0; }
            }
            ItemList* ans = new ItemList[1];
            ans->Len = NumOfDeficitDetails;
            ans->error = LIST_OK;
            int j = 0;
            ans->Names = new std::string[NumOfDeficitDetails];
            ans->HowManyNeed = new int[NumOfDeficitDetails];
            for(int k = 0; k < DeficitLen; ++k) {
                if(DeficitArray[k]) { 
                    ans->Names[j] = (*AllItems)[I.CompIndexes[k]].Name;
                    ans->HowManyNeed[j] = DeficitArray[k];
                    j++;
                }
            }
            delete [] DeficitArray;
            return ans;
        }

        bool create(std::string name, int quant) {  //Создаёт quant устройств имени name,
            int hmct = howManyCanCreate(name);
            if (hmct == THERE_IS_NOT_THAT_DETAIL || hmct < quant || hmct == DETAIL_CANT_BE_CREATED) { return false; }
            int ind = NameIndex->find(name);
            Item &A = (*AllItems)[ind];
            for(int i = 0; i < A.CompLen; ++i) {
                (*AllItems)[A.CompIndexes[i]].Quant -= quant * A.CompQuants[i];
            }
            changeQuantDelta(name, quant);
            return true;
        }

        bool canWeDelete(std::string name) {
            int ind = NameIndex->find(name);
            for(int i = 0; i < AllItems->size(); ++i) {
                if(!AllItems->isFree(i)) {
                    Item &I = (*AllItems)[i];
                    if(isThere(I.CompIndexes, I.CompLen, ind)) { return false; }
                }
            }
            return true;
        }

        int delItem(std::string name) { //удаляет устройство только если оно не используется для создания других
            int ind = NameIndex->find(name);
            if(ind < 0) { return -1; } //деталь не найдена на складе
            if(!canWeDelete(name)) { return 0; }
            AllItems->del(ind);
            NameIndex->del(name);
            return 1;
        }

        bool addNewItem(std::string name, int quant, int complen, std::string* compnames, int* compquants) {
            if(NameIndex->find(name) >= 0) {return false; }
            if(complen > 0) {
                if(areThereNegaivesOrZero(compquants, complen)) { return false; }
                if(areThereSimilarsStr(compnames, complen)) { return false; }
                int* ids = new int[complen];
                for(int i = 0; i < complen; i++) {
                    ids[i] = NameIndex->find(compnames[i]);
                    if (ids[i] == -1) {
                        printf("there is not such detail\n"); 
                        return false; 
                        }
                }
                Item C(1, name, quant, complen, ids, compquants);
                int ind = AllItems->add(C);
                NameIndex->add(C.Name, ind); 
                delete [] ids;
                return true;
                }
            if(complen == 0) { 
                Item C(1, name, quant); 
                int ind = AllItems->add(C);
                NameIndex->add(C.Name, ind);
                return false; 
                }
            return true;
        }

        bool change(std::string name, int quant, int complen, std::string* compnames, int* compquants) {
            int ind = NameIndex->find(name);
            if(ind < 0 || complen < 0 || quant < 0) { return false; }
            if(complen == 0) {
                AllItems->change(ind, quant, 0, nullptr, nullptr);
                return true; 
                }
            if(areThereNegaivesOrZero(compquants, complen)) { return false; }
            if(areThereSimilarsStr(compnames, complen)) { return false; }
            int* ids = new int[complen];
            for(int i = 0; i < complen; i++) {
                ids[i] = NameIndex->find(compnames[i]);
                if (ids[i] == -1) {
                    PushItem(compnames[i], 0, 0, nullptr, nullptr);
                    ids[i] = NameIndex->find(compnames[i]);
                    }
                } 
            AllItems->change(ind, quant, complen, ids, compquants);
            delete [] ids;
            return true;
        }
        
        bool changeOnlyQuant(std::string name, int quant) {
            if(quant < 0) { return false; } 
            int ind = NameIndex->find(name);
            if(ind < 0) { return false; }
            AllItems->changeOnlyQuant(ind, quant);
            return true;
        }

        bool changeQuantDelta(std::string name, int delta) {
            int id = NameIndex->find(name);
            if(id < 0) { return false; }
            int now = (*AllItems)[id].Quant;
            int shouldbe = now + delta;
            if (shouldbe < 0) { return false; }
            changeOnlyQuant(name, shouldbe);
            return true;
        }

        bool addComp(std::string name, int complen, std::string* compnames, int* compquants) {
            int ind = NameIndex->find(name);
            if(ind < 0 || complen <= 0 || compnames == nullptr || compquants == nullptr ) { return 0; }
            if(areThereNegaivesOrZero(compquants, complen)) { return false; }
            if(areThereSimilarsStr(compnames, complen)) { return false; }
            Item& I = (*AllItems)[ind];
            int NewLen = complen + I.CompLen;
            int* NewIndexes = new int[NewLen];
            for(int i = 0; i < complen; ++i) {
                int place = NameIndex->find(compnames[i]);
                if(place < 0) { delete[] NewIndexes; return false; }
                NewIndexes[i] = place;
            }
            if(compareInd(NewIndexes, complen, I.CompIndexes, I.CompLen)) { delete [] NewIndexes; return 0; }
            for(int j = complen; j < complen + I.CompLen; ++j) {
                NewIndexes[j] = I.CompIndexes[j - complen];
            }
            std::string* NewNames = new std::string[NewLen];
            for(int k = 0; k < NewLen; ++k) {
                NewNames[k] = (*AllItems)[NewIndexes[k]].Name;
            }
            int* NewQuants = new int[NewLen];
            for(int k = 0; k < I.CompLen; ++k) {
                NewQuants[k] = I.CompQuants[k];
            }
            for(int k = I.CompLen; k < NewLen; ++k) {
                NewQuants[k] = compquants[k-I.CompLen];
            }
            change(name, I.Quant, NewLen, NewNames, NewQuants);
            delete [] NewQuants; delete [] NewNames; delete [] NewIndexes;
            return true;
        }
        
        bool delComp(std::string name, int complen, std::string* compnames) {
            int ind = NameIndex->find(name);
            if(ind < 0 || complen <= 0 || compnames == nullptr ) { return false; }
            if(areThereSimilarsStr(compnames, complen)) { return false; }
            Item& I = (*AllItems)[ind];
            if(I.CompLen < complen) { return false; }
            int NewLen = I.CompLen - complen;
            if(NewLen == 0) {
                delete [] (*AllItems)[ind].CompIndexes;
                delete [] (*AllItems)[ind].CompQuants;
                (*AllItems)[ind].CompLen = 0;
                return true;
            }
            int* InpIndexes = new int[complen];
            for(int i = 0; i < complen; ++i) {
                int place = NameIndex->find(compnames[i]);
                if(place < 0) { delete[] InpIndexes; return false; }
                InpIndexes[i] = place;
            }
            if(compareInd(InpIndexes, complen, I.CompIndexes, I.CompLen) < complen) { delete [] InpIndexes; return 0; }
            std::string* NewNames = new std::string[NewLen];
            int* NewQuants = new int[NewLen];
            int i = 0;
            for(int l = 0; l < I.CompLen; ++l) {
                if(!isThere(InpIndexes, complen, I.CompIndexes[l])) {
                    NewNames[i] = (*AllItems)[I.CompIndexes[l]].Name;
                    NewQuants[i] = I.CompQuants[l];
                    ++i;
                }
            }
            change(name, I.Quant, NewLen, NewNames, NewQuants);
            delete [] InpIndexes; delete [] NewQuants; delete [] NewNames;
            return true;
        }

        bool PushItem(std::string name, int quant, int complen, std::string *compnames, int *compquants)
        {
            if (NameIndex->find(name) < 0)
            {
                if (complen > 0)
                {
                    int *ids = new int[complen];
                    for (int i = 0; i < complen; i++)
                    {
                        ids[i] = NameIndex->find(compnames[i]);
                        if (ids[i] == -1)
                        {
                            PushItem(compnames[i], 0, 0, nullptr, nullptr);
                        }
                        ids[i] = NameIndex->find(compnames[i]);
                    }
                    if (areThereSimilarsStr(compnames, complen))
                    {
                        delete[] ids;
                        return false;
                    }
                    Item C(1, name, quant, complen, ids, compquants);
                    int ind = AllItems->add(C);
                    NameIndex->add(C.Name, ind);
                    delete[] ids;
                    return true;
                }
                if (complen == 0)
                {
                    Item C(1, name, quant);
                    int ind = AllItems->add(C);
                    NameIndex->add(C.Name, ind);
                    return true;
                }
                return false;
            }
            if(NameIndex->find(name) >= 0) {
                change(name, quant, complen, compnames, compquants);
                return true;
            }
            return false;
        }

        ItemList* info(std::string name) const {
            int ind = NameIndex->find(name);
            if(ind < 0) { return nullptr; }
            Item &I = (*AllItems)[ind]; 
            ItemList* ans = new ItemList[1];
            ans->Name = I.Name;
            ans->Quant = I.Quant;
            ans->error = LIST_OK;
            ans->Len = I.CompLen;
            if(I.CompLen == 0) { return ans; }
            ans->HowManyNeed = new int[I.CompLen];
            ans->Names = new std::string[I.CompLen];
            for(int k = 0; k < I.CompLen; ++k) {
                ans->HowManyNeed[k] = I.CompQuants[k];
                ans->Names[k] = (*AllItems)[I.CompIndexes[k]].Name;
            }
            return ans;
        }

        int print() {
            AllItems->print();
            return 1;
        }
};

int min(int* array, int len) {
    int min = array[0];
    for(int i = 1; i < len; ++i) {
        if(array[i] < min) { min = array[i]; }
    }
    return min;
}

int compareInd(int* A, int lenA, int* B, int lenB) {
    int NumOfSimNum = 0;
    for(int i = 0; i < lenA; ++i) {
        for(int j = 0; j < lenB; ++j) {
            if (A[i] == B[j]) { NumOfSimNum++;}
        }
    }
    return NumOfSimNum;
}

int compareStr(std::string* A, int lenA, std::string* B, int lenB) {
    int NumOfSimNum = 0;
    for(int i = 0; i < lenA; ++i) {
        for(int j = 0; j < lenB; ++j) {
            if (A[i] == B[j]) { NumOfSimNum++;}
        }
    }
    return NumOfSimNum;
}

int areThereSimilars(int* A, int lenA) {
    for(int i = 0; i < lenA; ++i) {
        for(int j = 0; j < lenA; ++j) {
            if(A[i] == A[j] && i != j) {
                return 1;
            }
        }
    }
    return 0;
}

int areThereSimilarsStr(std::string* A, int lenA) {
    for(int i = 0; i < lenA; ++i) {
        for(int j = 0; j < lenA; ++j) {
            if((A[i] == A[j]) && (i != j)) {
                return 1;
            }
        }
    }
    return 0;
}

int isThere(int* A, int lenA, int X) {
    if(lenA <= 0 || A == nullptr) { return 0; }
    for(int j = 0; j < lenA; ++j) {
        if(A[j] == X) { return 1; }
    }
    return 0;
}

bool areThereNegaivesOrZero(int* A, int lenA) {
    int NumOfSimNum = 0;
    for(int i = 0; i < lenA; ++i) {
        if(A[i] <= 0) { return true; }
    }
    return false;
}

