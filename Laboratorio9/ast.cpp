#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
  switch (op) {
  case PLUS_OP:
    return "+";
  case MINUS_OP:
    return "-";
  case MUL_OP:
    return "*";
  case DIV_OP:
    return "/";
  case POW_OP:
    return "**";
  case LE_OP:
    return "<";
  case AND_OP:
    return "and";
  default:
    return "?";
  }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp *l, Exp *r, BinaryOp o) : left(l), right(r), op(o) {}

BinaryExp::~BinaryExp() {
  delete left;
  delete right;
}

// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}

// ------------------idExp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}

Stm::~Stm() {}

PrintStm::~PrintStm() {}

IfStm::~IfStm() {}

AssignStm::~AssignStm() {}

Program::~Program() {}

PrintStm::PrintStm(Exp *expresion) { e = expresion; }

IfStm::IfStm(Exp *e, Body *b1, Body *b2) {
  cond = e;
  trueB = b1;
  elseB = b2;
}

AssignStm::AssignStm(string variable, Exp *expresion) {
  id = variable;
  e = expresion;
}

Program::Program() {}
