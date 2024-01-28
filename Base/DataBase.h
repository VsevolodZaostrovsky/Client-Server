#pragma once
#include <string.h>
#include "Storage.h"
#include "SyntaxAnal.h"

typedef enum { STORE_OK = 0, SYNTAX_ERROR = 1, DATA_ERROR = 2 } ErrorType;
typedef size_t UserID;

int writeStr(char* start, std::string str, int len) {
    if(len < str.size()) { return 0; }
    for(int i = 0; i < str.size(); ++i) {
        start[i] = str[i];
    }
    return 1;
}

struct Result {
    ErrorType error;
    CommandType cmd;
    UserID user;
    int num;    //в командах ISPOSSIBLE и HOWMANy обозначает возможность создания детали и количество деталей, которые возможно создать, соответственно

    std::string ItemName;
    ItemList ans;         //в зависимости от запроса, здесь хранится сборочная карта либо список дефицитных деталей (либо NULL)

    void view() {
        if(cmd == ERROR_COMMAND) { 
            printf("Incorrect command (Syntax):\n");
            return; 
            }
        if(cmd == HOWMANY) { 
            if(num > 0) { printf("HOWMANY:\nit is possible to create %d\n", num); }
            if(num == 0) { printf("HOWMANY:\nnot enough detail to create even one detail\n"); }
            if(num == -1) { printf("HOWMANY:\nThis detail has not construction card\n"); }
            if(num == -2) { printf("HOWMANY:\nThere is not such detail in storage\n"); }
            return; 
            }
        if(cmd == PRINT) {
            printf("PRINT:\n");
            std::cout << "Name: "<< ItemName << std::endl; 
            std::cout << "Quant: "<< num << std::endl;
            ans.print(); 
            return; 
            }
        if(cmd == ISPOSSIBLE) {
            printf("ISPOSSIBLE:\n");
            if(num == 1) { printf("can create\n to create 1 more:\n");}
            if(num == -1) { printf("This detail has not construction card OR There is not such detail in storage"); return; }
            else { printf("cannot create\n to create:\n");}
            ans.print(); 
            return; 
            }
        
        else {
            switch (cmd)
            {
            case DELITEM: printf("DELITEM "); break;
            case DEL: printf("DEL "); break;
            case ADD: printf("ADD "); break;
            case ADDITEM: printf("ADDITEM "); break;
            case CREATE: printf("CREATE "); break;
            case ADDCOMP: printf("ADDCOMP "); break;
            case DELCOMP: printf("DELCOMP "); break;
            }
            if(num) { printf(" done\n");}
            else { printf(" hasnt done\n");}
            return; }
    }

    std::string toString(int buflen) {
        std::string code;
        if(cmd == ERROR_COMMAND) {
            code = "-1 Synt_error";
        }
        if(cmd == HOWMANY) {
            code = "2 HOWMANY ";
            code += std::to_string(num);
        }
        if(cmd == DEL) {
            code = "4 Del ";
            code += std::to_string(num);
        }
        if(cmd == ADD) {
            code = "5 ADD ";
            code += std::to_string(num);
        }
        if(cmd == DELITEM) {
            code = "3 DELITEM ";
            code += std::to_string(num);
        }
        if(cmd == ADDITEM) {
            code = "6 ADDITEM ";
            code += std::to_string(num);
        }
        if(cmd == CREATE) {
            code = "7 CREATE ";
            code += std::to_string(num);
        }
        if(cmd == ADDCOMP) {
            code = "10 ADDCOMP ";
            code += std::to_string(num);
        }
        if(cmd == DELCOMP) {
            code = "11 DELCOMP ";
            code += std::to_string(num);
        }
        if(cmd == PRINT) {
            code = (std::string)ans.Name;
            if(error == DATA_ERROR) {
                code = "DATA_ERROR";
                return code;
            }
            code += ItemName; 
            code += " "; 
            code += std::to_string(num);
            for(int i = 0; i < ans.Len; ++i) {
                code += " "; 
                code += ans.Names[i]; 
                code += " ";
                code += std::to_string(ans.HowManyNeed[i]);
            }
        }
        if(cmd == ISPOSSIBLE) {
            code = (std::string)ans.Name;
            if(error == DATA_ERROR) {
                code = "DATA_ERROR";
                return code;
            }
            code += ItemName; 
            code += " "; 
            code += std::to_string(num);
            for(int i = 0; i < ans.Len; ++i) {
                code += " "; 
                code += ans.Names[i]; 
                code += " ";
                code += std::to_string(ans.HowManyNeed[i]);
            }
        }
        return code;
    }
};

class DataBase {
    public:
        Storage Store;

        Result execute(Command A, UserID user) {
            Result Res;
            Res.cmd = A.cmd;
            Res.user = user;
            int posQuant = 0;
            int neededQuant = 0;
            if(A.cmd == ERROR_COMMAND) { 
                return Res; 
                }
            std::string Aname;
            int flag1 = 0;
            int NumOfComps = 0;
            std::string * CompNames = nullptr; int * CompQuants = nullptr;
            for (auto iter = A.conditions.begin(); iter != A.conditions.end(); iter++) {
                if((*iter).field == NAME) { Aname = (*iter).s; }
                if((*iter).field == QUANTITY) { neededQuant = (*iter).i; }
                if((*iter).field == COMP) { NumOfComps++; }
                }
            if(NumOfComps > 0) {
                CompNames = new std::string[NumOfComps];
                CompQuants = new int[NumOfComps];
                int k = 0;
                for (auto iter = A.conditions.begin(); iter != A.conditions.end(); iter++) {
                    if((*iter).field == COMP) { CompNames[k] = (*iter).s; }
                    if((*iter).field == COMP_QUANT) { CompQuants[k] = (*iter).i; k++; }
                }
            }
            ItemList* Inf = Store.info(Aname);
            if (A.cmd != PRINT && A.cmd != ISPOSSIBLE) { delete [] Inf; }
            switch (A.cmd)
            {
            case ISPOSSIBLE:
                posQuant = Store.howManyCanCreate(Aname);
                if(posQuant == THERE_IS_NOT_THAT_DETAIL) { Res.error = DATA_ERROR; flag1 = 1; }
                if(posQuant == DETAIL_CANT_BE_CREATED) { Res.error = DATA_ERROR; flag1 = 1;  }
                if(neededQuant <= 0) { Res.error = SYNTAX_ERROR; flag1 = 1; }
                if(flag1) { 
                    Res.num = -1;
                    delete [] Inf;
                    return Res;
                    }
                if(posQuant >= neededQuant) { 
                    Res.num = 1; 
                    Res.error = STORE_OK;
                    delete [] Inf;
                    Inf = Store.deficitAfterCreate(Aname, neededQuant);
                    Res.error = STORE_OK;
                    if(Inf !=nullptr && Inf->Len > 0) {
                        Res.ans.Len = Inf->Len;
                        Res.ans.Names = new std::string[Inf->Len];
                        Res.ans.HowManyNeed = new int[Inf->Len];
                        for(int k = 0; k < Inf->Len; ++k) {
                            Res.ans.Names[k] = Inf->Names[k];
                            Res.ans.HowManyNeed[k] = Inf->HowManyNeed[k];
                        }
                        }
                    delete [] Inf;
                    Res.error = STORE_OK;
                    return Res; 
                    }
                delete [] Inf;
                Inf = Store.whatWeNeedToCreate(Aname, neededQuant);
                Res.error = STORE_OK;
                if(Inf !=nullptr && Inf->Len > 0) {
                    Res.ans.Len = Inf->Len;
                    Res.ans.Names = new std::string[Inf->Len];
                    Res.ans.HowManyNeed = new int[Inf->Len];
                    for(int k = 0; k < Inf->Len; ++k) {
                        Res.ans.Names[k] = Inf->Names[k];
                        Res.ans.HowManyNeed[k] = Inf->HowManyNeed[k];
                    }
                }
                delete [] Inf;
                Res.error = STORE_OK;
                Res.num = 0;
                return Res;

            case HOWMANY:
                posQuant = Store.howManyCanCreate(Aname);
                if(posQuant < 0) { Res.error = DATA_ERROR; }
                Res.num = posQuant;
                Res.error = STORE_OK;
                return Res;

            case DELITEM:
                Res.num = Store.delItem(Aname);
                Res.error = STORE_OK;
                return Res;
            
            case DEL:
                Res.num = Store.changeQuantDelta(Aname, -neededQuant);
                Res.error = STORE_OK;
                return Res;

            case ADD:
                Res.num = Store.changeQuantDelta(Aname, neededQuant);
                Res.error = STORE_OK;
                return Res;

            case ADDITEM:
                if(Store.PushItem(Aname, neededQuant, NumOfComps, CompNames, CompQuants)) {
                    Res.error = STORE_OK;
                    Res.num = 1;
                }
                else { Res.error = DATA_ERROR; Res.num = 0; }
                delete [] CompNames; delete [] CompQuants;
                return Res;

            case CREATE:
                Res.num = Store.create(Aname, neededQuant);
                Res.error = STORE_OK;
                if(Res.num < 0) { Res.error = DATA_ERROR; }
                return Res;
                break;

            case PRINT:
                if(Inf == nullptr) { Res.error = DATA_ERROR; return Res; }
                Res.error = STORE_OK;
                Res.ItemName = Inf->Name;
                Res.num = Inf->Quant;
                if(Inf->Len > 0) {
                    Res.ans.Len = Inf->Len;
                    Res.ans.Names = new std::string[Inf->Len];
                    Res.ans.HowManyNeed = new int[Inf->Len];
                    for(int k = 0; k < Inf->Len; ++k) {
                        Res.ans.Names[k] = Inf->Names[k];
                        Res.ans.HowManyNeed[k] = Inf->HowManyNeed[k];
                    }
                }
                delete [] Inf;
                return Res;

            case ADDCOMP:
                if(Store.addComp(Aname, NumOfComps, CompNames, CompQuants)) {
                    Res.error = STORE_OK;
                    Res.num = 1;
                }
                else { Res.error = DATA_ERROR; Res.num = 0; }
                delete [] CompNames; delete [] CompQuants;
                return Res;
            
            case DELCOMP:
                if(Store.delComp(Aname, NumOfComps, CompNames)) {
                    Res.error = STORE_OK;
                    Res.num = 1;
                }
                else { Res.error = DATA_ERROR; Res.num = 0; }
                delete [] CompNames; delete [] CompQuants;
                return Res;
            }
            return Res;
        }


    public:
        DataBase() = default;
        ~DataBase() = default;

        void print() {
            Store.print();
        }

        void addFromFile(std::string filename) {
            if(Store.readFrom(filename) != 1) {
                printf("Error - addFromFile\n");
            }
        }

        void writeTo(std::string filename) {
            Store.writeTo(filename);
        }
 
        Result execute(std::string command, UserID user) {
            return execute(parse(command), user);
        }
};