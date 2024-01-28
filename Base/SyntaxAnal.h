#pragma once
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

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

typedef enum
{
  NAME = 0,
  QUANTITY = 1,
  COMP = 2,
  CREATED = 3,
  COMP_QUANT = 5
} Field;

struct Cond
{ // Одно условие вида поле/отношение/константа
  Field field;
  int i;
  std::string s;

  Cond(Field f, int in) : field(f), i(in) {}

  Cond(Field f, std::string st) : field(f), s(st) {}

  friend std::ostream &operator<<(std::ostream &os, const Cond &C)
  {
    if (C.field % 2 == 1) { std::cout << C.i; }
    else { std::cout << C.s; }
    return os;
  }

  bool operator==(const Cond &C) const
  {
    if (field == C.field)
    {
      if (C.field % 2 == 0) { if (i == C.i) { return 1; } return 0; }
      else { if (s == C.s) { return 1; } return 0; }
    }
    return 0;
  }
};

typedef std::list<Cond> SearchConditions;

struct Command
{
  CommandType cmd = ERROR_COMMAND;
  SearchConditions conditions;

  int view()
  {
    std::cout << cmd << std::endl;
    copy(conditions.begin(), conditions.end(), std::ostream_iterator<Cond>(std::cout, " "));
    std::cout << std::endl;
    return 1;
  }

  int add(Field f, int in)
  {
    Cond C(f, in);
    conditions.emplace_back(C);
    return 1;
  }

  int add(Field f, std::string s)
  {
    Cond C(f, s);
    conditions.emplace_back(C);
    return 1;
  }
};

int lenBeforeSpace(const std::string &query, int start) {
  int ans = 0;
  for(int k = start; query[k] != ' '; ++k) { ans++;}
  return ans;
}

int lenBeforeSpaceOrEq(const std::string &query, int start) {
  int ans = 0;
  for(int k = start; query[k] != ' ' && query[k] != '=' && query[k] != '\0'; ++k) { ans++;}
  return ans;
}

int WhatCommand(const std::string &query) {
  std::string word;
  int qlen = query.length();
  if(qlen < 4) { return -1; }
  word.assign(query, 0, 4);
  if(word == "DEL ") { return DEL; }
  if(word == "ADD ") { return ADD; }
  if(qlen < 6) { return -1; }
  word.assign(query, 0, 6);
  if(word == "PRINT ") { return PRINT; }
  if(qlen < 7) { return -1; }
  word.assign(query, 0, 7);
  if(word == "CREATE ") { return CREATE; }
  if(qlen < 8) { return -1; }
  word.assign(query, 0, 8);
  if(word == "HOWMANY ") { return HOWMANY; }
  if(word == "DELITEM ") { return DELITEM; }
  if(word == "ADDITEM ") { return ADDITEM; }
  if(word == "ADDCOMP ") { return ADDCOMP; }
  if(word == "DELCOMP ") { return DELCOMP; }
  if(qlen < 11) { return -1; }
  word.assign(query, 0, 11);
  if(word == "ISPOSSIBLE ") { return ISPOSSIBLE; }
  return -1;
}

bool isThatKeyWord(const std::string word) {
  if(word == "DEL" || word == "ADD" || word == "PRINT" || word == "CREATE" || word == "HOWMANY"
    || word == "DELITEM" || word == "ADDITEM" || word == "ADDCOMP" || word == "DELCOMP" 
    || word == "ISPOSSIBLE" || word == "NAME" || word == "QUANTITY" || word == "END" 
    || word == "CREATED" || word == "COMP") { 
      return true;
    }
  return false;
}

bool toUnite(std::string *q, int &len){
  int numKW = 0;
  for(int k = 0; k < len; ++k) {
    if(isThatKeyWord(q[k])) { numKW++; }
  }
  int UniteLen = (numKW - 2) * 2 + 1;
  return true;
}

std::string* Reading(const std::string &query, int &len) {
  int wordNum = 1;
  for(int now = 0; now < query.length(); now++) {
    if(query[now] == ' ' || query[now] == '=') {
      wordNum += 1;
    }
    len = wordNum;
  }
  std::string* strarr = new std::string[wordNum];
  int now = 0;
  for(int word = 0; word < wordNum; word++) {
    strarr[word].assign(query, now, lenBeforeSpaceOrEq(query, now));
    now = now + lenBeforeSpaceOrEq(query, now) + 1;
//    std::cout << word << " iteration now = "<< now << std::endl;
  }  
  return strarr;
}

Command parISPOSSIBLE(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND;
  int len = 0;
  std::string * sar = Reading(query, len);
  if(len != 6 || sar[0] != "ISPOSSIBLE" || sar[1] != "NAME" || sar[3]!= "QUANTITY" || sar[5] != "END") { return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.add(QUANTITY, atoi(sar[4].c_str()));
  Ans.cmd = ISPOSSIBLE;
  delete [] sar;
  return Ans;
}

Command parHOWMANY(const std::string &query) { 
  Command Ans; Ans.cmd = ERROR_COMMAND; int len = 0;
  std::string * sar = Reading(query, len);
  if(len != 4 || sar[0] != "HOWMANY" || sar[1] != "NAME" || sar[3]!= "END") { return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.cmd = HOWMANY;
  delete [] sar;
  return Ans;
 }

Command parPRINT(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND; int len = 0;
  std::string * sar = Reading(query, len);
  if(len != 4 || sar[0] != "PRINT" || sar[1] != "NAME" || sar[3] != "END") { return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.cmd = PRINT;
  delete [] sar;
  return Ans;
}

Command parDELITEM(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND; int len = 0;
  std::string * sar = Reading(query, len);
  if(len != 4 || sar[0] != "DELITEM" || sar[1] != "NAME" || sar[3] != "END") { return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.cmd = DELITEM;
  delete [] sar;
  return Ans;
}

Command parDEL(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND;
  int len = 0;
  std::string * sar = Reading(query, len);
  if(len != 6 || sar[0] != "DEL" || sar[1] != "NAME" || sar[3]!= "QUANTITY" || sar[5] != "END") { return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.add(QUANTITY, atoi(sar[4].c_str()));
  Ans.cmd = DEL;
  delete [] sar;
  return Ans;
}

Command parADD(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND;
  int len = 0;
  std::string * sar = Reading(query, len);
  if(len != 6 || sar[0] != "ADD" || sar[1] != "NAME" || sar[3]!= "QUANTITY" || sar[5] != "END") { return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.add(QUANTITY, atoi(sar[4].c_str()));
  Ans.cmd = ADD;
  delete [] sar;
  return Ans;
}

Command parCREATE(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND;
  int len = 0;
  std::string * sar = Reading(query, len);
  if(len != 6 || sar[0] != "CREATE" || sar[1] != "NAME" || sar[3]!= "QUANTITY" || sar[5] != "END") { return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.add(QUANTITY, atoi(sar[4].c_str()));
  Ans.cmd = CREATE;
  delete [] sar;
  return Ans;
}

Command parADDITEM(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND;
  int len = 0;
  std::string * sar = Reading(query, len);
  if(sar[0] != "ADDITEM" || sar[1] != "NAME" || sar[3]!= "QUANTITY" 
      || sar[5] != "CREATED" || sar[len-1] != "END") { delete [] sar; return Ans; }
  Ans.add(NAME, sar[2]);
  Ans.add(QUANTITY, atoi(sar[4].c_str()));
  if(sar[6] == "0") { 
    Ans.add(CREATED, atoi(sar[6].c_str())); 
    Ans.cmd = ADDITEM;
    delete [] sar;
    return Ans;
    }
  if(sar[6] != "1") { delete [] sar; return Ans; }
  Ans.add(CREATED, atoi(sar[6].c_str()));
  for (int i = 8; i < len; i+=4) {
    Ans.add(COMP, sar[i]);
    Ans.add(COMP_QUANT, atoi(sar[i+2].c_str()));
  }
  Ans.cmd = ADDITEM;
  delete [] sar;
  return Ans;
}

Command parADDCOMP(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND;
  int len = 0;
  std::string * sar = Reading(query, len);
  if(sar[0] != "ADDCOMP" || sar[1] != "NAME" || sar[len-1] != "END") { delete [] sar; return Ans; }
  Ans.add(NAME, sar[2]);

  for (int i = 4; i < len; i+=4) {
    Ans.add(COMP, sar[i]);
    Ans.add(COMP_QUANT, atoi(sar[i+2].c_str()));
  }
  Ans.cmd = ADDCOMP;
 /* for(int i = 0; i < len; ++i) {
    std::cout << i << " " << sar[i] << std::endl;
  } */
  delete [] sar;
  return Ans;
}

Command parDELCOMP(const std::string &query) {
  Command Ans; Ans.cmd = ERROR_COMMAND;
  int len = 0;
  std::string * sar = Reading(query, len);
  if(sar[0] != "DELCOMP" || sar[1] != "NAME" || sar[len-1] != "END") { delete [] sar; return Ans; }
  Ans.add(NAME, sar[2]);

  for (int i = 4; i < len; i+=2) {
    Ans.add(COMP, sar[i]);
  }
  Ans.cmd = DELCOMP;
 /* for(int i = 0; i < len; ++i) {
    std::cout << i << " " << sar[i] << std::endl;
  } */
  delete [] sar;
  return Ans;
}

Command parse(const std::string &query) {
  int type = WhatCommand(query);
  Command C;
  switch (type)
  {
  case ISPOSSIBLE: C = parISPOSSIBLE(query); return C;
  case HOWMANY: C = parHOWMANY(query); return C;
  case DELITEM: C = parDELITEM(query); return C;
  case DEL: C = parDEL(query); return C;
  case ADD: C = parADD(query); return C;
  case ADDITEM: C = parADDITEM(query); return C;
  case CREATE: C = parCREATE(query); return C;
  case PRINT: C = parPRINT(query); return C;
  case ADDCOMP: C = parADDCOMP(query); return C;
  case DELCOMP: C = parDELCOMP(query); return C;;
  }
  C.cmd = ERROR_COMMAND;
  return C;
}