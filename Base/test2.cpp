#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include "SyntaxAnal.h"

int main() {
    std::string str = "UPDATE NAME=KUE COMP=ASD COMP=A12D END";
    parse(str).view();
    int i = 16;
    int len = 0;
    std::string * array = Reading(str, len);
//    std::cout << "len = " << len << std::endl;
    delete [] array;
    return 1;
}