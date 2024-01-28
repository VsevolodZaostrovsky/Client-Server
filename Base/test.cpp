#include <string>
#include "Storage.h"
#include "Cont.h"
#include "HashSet.h"


int main() {
    Storage Str;
    Str.readFrom("Test.txt");
    Str.print();
//    Str.change("A", 10, 0, nullptr, nullptr);
//    Str.print();
    return 0; 
}





/*    Str.addNewItem("A", 5, 0, nullptr, nullptr);
    std::string* nameA = new std::string[1];
    nameA[0] = "A";
    int* quant = new int[1];
    quant[0] = 2;
    Str.addNewItem("B", 1, 1, nameA, quant);
    Str.addNewItem("C", 2, 0, nullptr, nullptr);
    Str.addNewItem("D", 3, 0, nullptr, nullptr);
    Str.addNewItem("E", 4, 0, nullptr, nullptr);
    Str.change("C", 1555, 1, nameA, quant);
    Str.change("E", 15, 1, nameA, quant);
    int *cq = new int[2]; cq[0] = 11; cq[1] = 2;
    std::string* cn = new std::string[2];
    cn[0] = "E", cn[1] = "D";
    Str.addComp("C", 2, cn, cq);
    Str.print();
    Str.writeTo("Test.txt"); */