#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include "SyntaxAnal.h"
#include "DataBase.h"

int main() {
    DataBase A;
    A.addFromFile("Test.txt");
    A.execute("DELITEM NAME=N END", 1);
    A.execute("DEL NAME=C QUANTITY=50 END", 1);
    A.execute("ADD NAME=C QUANTITY=70 END", 1);
    A.execute("PRINT NAME=C END", 1);
    A.execute("ADDITEM NAME=NEW_ITEM QUANTITY=30 CREATED=0 END", 1).view();
    A.execute("ADDITEM NAME=NEW_ITEM2 QUANTITY=31 CREATED=1 COMP=NEW_ITEM COMP_QUANT=1 COMP=ITEM2 COMP_QUANT=112 COMP=ITEM3 COMP_QUANT=11212 END", 1).view();
    A.execute("DELITEM NAME=NEW_ITEM2 END", 1).view();
    A.execute("DELITEM NAME=NEW_ITEM END", 1).view();
    A.execute("DELITEM NAME=ITEM2 END", 1).view();
    A.execute("DELITEM NAME=ITEM3 END", 1).view();
    A.execute("DELITEM NAME=ABCDEc END", 1).view();
    A.execute("DELITEM NAME=D END", 1).view();
    A.execute("ADDITEM NAME=D QUANTITY=30 CREATED=0 END", 1).view();
    A.execute("ADDITEM NAME=F QUANTITY=30 CREATED=0 END", 1).view();
    A.execute("ADDCOMP NAME=E COMP=D QUANTITY=10 COMP=C Q=12 C=A Q=123 END", 1).view();
    A.execute("DELCOMP NAME=E COMP=D END", 1).view();
    A.execute("CREATE NAME=XX_1 QUANTITY=1 END", 1).view();
    A.addFromFile("large.txt");
    A.print();
    return 1;
}