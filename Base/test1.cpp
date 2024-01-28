#include <string>
#include "Storage.h"
#include "Cont.h"
#include "HashSet.h"


int main() {

    Cont A;
    index* indA = new index[2];
    indA[0] = 0; indA[1] = 2;

/*    int* quant = new int[2];
    quant[0] = 2; quant[1] = 4;
    Item I1(1, "A", 20);
    Item I2(1, "B", 2);
    Item I3(1, "C", 12);
    A.add(I1); A.add(I2); A.add(I3);
    A[1].print();
    A.change(1, 2, 2, indA, quant);
    A[1].print(); 
    indA[0] = 1;
    quant[0] = 3;
    A.change(1, 2, 1, indA, quant);
    A[1].print(); 
    int *Aarr = new int[4]; Aarr[0] = 1; Aarr[1] = 2; Aarr[2] = 10; Aarr[3] = 3;
    printf("Are there sim %d\n", areThereSimilars(Aarr, 4));
    int *Barr = new int[4]; Barr[0] = 1312; Barr[1] = 20; Barr[2] = 100; Barr[3] = 312;
    printf("Compering res %d\n", compare(Aarr, 4, Barr, 4));
    delete [] Aarr; delete [] Barr;*/


    Storage Str;
    Str.addNewItem("A", 5, 0, nullptr, nullptr);
    std::string* nameA = new std::string[1];
    nameA[0] = "A";
    int* quant = new int[1];
    quant[0] = 2;
    Str.addNewItem("B", 1, 1, nameA, quant);
    Str.addNewItem("C", 2, 0, nullptr, nullptr);
    Str.addNewItem("D", 3, 0, nullptr, nullptr);
    Str.addNewItem("E", 4, 0, nullptr, nullptr);
//    Str.print();
    Str.change("C", 1555, 1, nameA, quant); 
//    Str.print(); 
//    Str.find("C").print();
//    printf("B %d\n", Str.create("B", 1));
    ItemList* E = Str.whatWeNeedToCreate("B", 100);
//    E->print();
//    printf("can create %d B\n", Str.howManyCanCreate("B"));
//    Str.print(); 
    ItemList* D = Str.deficitAfterCreate("B", 1);
    if(D != nullptr) {
//    D->print();
    }
//    else { printf("D is empty\n"); }
    int *cq = new int[2]; cq[0] = 11; cq[1] = 2;
    std::string* cn = new std::string[2];
    cn[0] = "E", cn[1] = "D";
    Str.addComp("B", 2, cn, cq);
//    Str.print();
    cn[0] = "A";
//    Str.print();
    Str.delComp("B", 1, cn);
//    Str.print();
//    printf("Candel = %d\n", Str.canWeDelete("E"));
//    Str.print();
//    Str.delItem("B");
//    Str.delItem("C");
//    Str.delItem("A");
//    printf("dele\n\n");
    nameA[0] = "E";
    Str.addNewItem("B", 1, 1, nameA, quant);
    Str.addNewItem("F", 1, 0, nullptr, nullptr);
    Str.addNewItem("F3", 12, 0, nullptr, nullptr);
//    Str.print();
    Str.writeTo("Test.txt");
    delete [] cn;
    delete [] cq;
    delete [] E;
    delete [] D; 
    Str.print(); 
    delete [] quant;
    delete [] indA;
    delete [] nameA;
    return 0;
}