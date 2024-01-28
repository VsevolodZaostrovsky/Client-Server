#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <math.h>
typedef enum
{
  ERROR_COMMAND = -1,
  ISPOSSIBLE = 1,
  HOWMANY = 2,
  DELITEM = 3,
  DEL = 4,
  ADD = 5,
  ADDITEM = 6,
  CREATE = 7,
  PRINT = 8,
  ADDCOMP = 10,
  DELCOMP = 11
} CommandType;
#define ANY 666

int isThereTest(int* A, int lenA, int X) {
    if(lenA <= 0 || A == nullptr) { return 0; }
    for(int j = 0; j < lenA; ++j) {
        if(A[j] == X) { return 1; }
    }
    return 0;
}

int generateScenario(std::string filename, int comands, int ndevices, int comtypes = ANY) {
    std::ofstream out;
    out.open(filename);
    int com = 0;
    int parnum = 5;
    int* devices = new int[parnum];
    int* quantities = new int[parnum];
    int complen = 0;
    for(int i = 0; i < parnum; ++i) { 
        devices[i] = 0; 
        quantities[i] = 0; 
        }
    for(int k = 0; k < comands; k++) {
        com = abs(rand())%12;
        int val1 = abs(rand())%ndevices + 1;
        int val2 = abs(rand())%100 + 1;
        for(int parent = 0; parent < parnum; parent++) {
            int device = int(abs(rand()) % ndevices);
            if(!isThereTest(devices, parnum, device)) {
                devices[parent] = device;
                quantities[parent] = (abs(int(rand() * 10) + 1)) % 10;
                complen++;
            }
        }
        if(comtypes != ANY) { com = comtypes; }
        switch (com)
        {
        case ISPOSSIBLE:
            out << "ISPOSSIBLE " << "NAME=D_" << val1 << " QUANTITY=" << val2 << " END\n";
            break;
        case HOWMANY:
            out << "HOWMANY " << "NAME=D_" << val1 << " END\n";
            break;
        case DELITEM:
            out << "DELITEM " << "NAME=D_" << val1 << " END\n";
            break;
        case DEL:
            out << "DEL " << "NAME=D_" << val1 << " QUANTITY=" << val2 << " END\n";
            break;
        case ADD:
            out << "ADD " << "NAME=D_" << val1 << " QUANTITY=" << val2 << " END\n";
            break;
        case ADDITEM:
            out << "ADDITEM " << "NAME=D_" << val1 << " QUANTITY=" << val2;
            if((val1+val2)%2) { out<<" CREATED=0 END\n"; break; }
            out<<" CREATED=1";
            for(int parent = 0; parent < parnum; parent++) {
            if(devices[parent]) {
                out << " COMP=D_" << devices[parent] << " QUANTITY=" << quantities[parent];
                }
            }
            out << " END\n";
            break;
        case CREATE:
            out << "CREATE " << "NAME=D_" << val1 << " QUANTITY=" << val2 << " END\n";
            break;
        case ADDCOMP:
            out << "ADDCOMP " << "NAME=D_" << val1;
            for(int parent = 0; parent < parnum; parent++) {
            if(devices[parent]) {
                out << " COMP=D_" << devices[parent] << " QUANTITY=" << quantities[parent];
                }
            }
            out << " END\n";
            break;
        case DELCOMP:
            out << "DELCOMP " << "NAME=D_" << val1;
            for(int parent = 0; parent < parnum; parent++) {
            if(devices[parent]) {
                out << " COMP=D_" << devices[parent];
                }
            }
            out << " END\n";
            break;
        }
    }
    delete [] devices; delete [] quantities;
//    out << "HOWMANY NAME=D_12 END";
    out << "deactivate";
    out.close();
    return 1;
}

int main() {
    fprintf(stdout,"Add size of scenario > ");
    int len;
    std::cin >> len;
    generateScenario("scenario.txt", len, 30, ANY);
    return 0;
}