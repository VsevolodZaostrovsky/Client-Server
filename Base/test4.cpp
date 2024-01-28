#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <math.h>
#include "SyntaxAnal.h"
#include "DataBase.h"

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
            delete [] devices; delete [] quantities;
        }
        else { out << "CompLen: 0\n"; }
    }
    return 1;
}

int main() {
    DataBase A;
    generateRandomBase("large.txt", 10000);
    Result Re;
    A.addFromFile("large.txt");
    std::string command;
    while(true) {
        std::cout << "please, enter the command\n";
        std::getline(std::cin, command);
        if(command == "STOP") { return 0; }
        A.execute(command, 1).view();
    } 
    A.execute("ADDITEM NAME=NEW_ITEM QUANTITY=100 CREATED=0 END", 1).view();
    A.execute("ADDCOMP NAME=NEW_ITEM COMP=D_1 QUANTITY=1 COMP=D_2 QUANTITY=2 COMP=D_3 QUANTITY=3 END", 1);
    A.execute("ISPOSSIBLE NAME=NEW_ITEM QUANTITY=1 END", 1);
    return 0;
}