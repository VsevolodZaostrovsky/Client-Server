#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <math.h>
#include "SyntaxAnal.h"
#include "DataBase.h"

#define ANY 666

int isThereTest(int* A, int lenA, int X) {
    if(lenA <= 0 || A == nullptr) { return 0; }
    for(int j = 0; j < lenA; ++j) {
        if(A[j] == X) { return 1; }
    }
    return 0;
}

int generateRandomBase(std::string filename, int ndevices) {
    std::ofstream out;
    out.open(filename);
    int quant = 0;
    for(int k = 0; k < ndevices; k++) {
        quant = abs(rand())%55;
        out << "Name: D_" << k << "\nQuant: " << quant << "\n";
        if(rand() % 2) {
            int parnum = 5;
            int* devices = new int[parnum];
            int* quantities = new int[parnum];
            int complen = 0;
            for(int i = 0; i < parnum; ++i) { 
                devices[i] = 0; 
                quantities[i] = 0; 
                }
            for(int parent = 0; parent < parnum; parent++) {
                int device = int(abs(rand()) % ndevices);
                if(!isThereTest(devices, parnum, device)) {
                    devices[parent] = device;
                    quantities[parent] = (abs(int(rand() * 10) + 1)) % 10;
                    complen++;
                }
            }
            out << "CompLen: " << complen << std::endl;
            for(int parent = 0; parent < parnum; parent++) {
                int device = int(abs((ndevices * rand())) % parnum);
                if(devices[parent]) {
                    out << "D_" << devices[parent] << " " << quantities[parent] << "\n";
                }
            }
            delete [] devices;
            delete [] quantities;
        }
        else { out << "CompLen: 0\n"; }
    }
    out.close();
    return 1;
}

int generateCommandFile(std::string filename, int comands, int ndevices, int comtypes = ANY) {
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
    out.close();
    return 1;
}

int main() {
    DataBase A;
    generateRandomBase("large.txt", 30);
    generateCommandFile("commands.txt", 10000, 30, ANY);
    A.addFromFile("large.txt");
    std::ifstream inp("commands.txt");
    int p = inp.is_open();
    if (!p) { inp.close(); return false; }
    int donecom = 0;
    std::string command;
    Result R;
    printf("while started\n"); 
    while (!inp.eof()) {
        std::getline(inp, command);
//        printf("%din ", donecom+1);
       A.execute(command, 1);
        command = "";
        donecom++;
    } 
//    A.execute("ISPOSSIBLE NAME=D_15 QUANTITY=3 END", 1).view();
    printf("%d commands was executed\n", donecom); 
    inp.close();
    return 0;
}