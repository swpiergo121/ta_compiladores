#include "visitor.h"
#include "ast.h"
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 1: MÉTODOS accept() DEL AST
///////////////////////////////////////////////////////////////////////////////////

// Expresiones
int BinaryExp::accept(Visitor *visitor) { return visitor->visit(this); }
int NumberExp::accept(Visitor *visitor) { return visitor->visit(this); }
int IdExp::accept(Visitor *visitor) { return visitor->visit(this); }
int NotExp::accept(Visitor *visitor) { return visitor->visit(this); }
int NegExp::accept(Visitor *visitor) { return visitor->visit(this); }
int FcallExp::accept(Visitor *visitor) { return visitor->visit(this); }
int BoolExp::accept(Visitor *visitor) { return visitor->visit(this); }
// Sentencias
int PrintStm::accept(Visitor *visitor) { return visitor->visit(this); }
int IfStm::accept(Visitor *visitor) { return visitor->visit(this); }
int AssignStm::accept(Visitor *visitor) { return visitor->visit(this); }
int ReturnStm::accept(Visitor *visitor) { return visitor->visit(this); }

// Declaraciones
int VarDec::accept(Visitor *visitor) { return visitor->visit(this); }
int FunDec::accept(Visitor *visitor) { return visitor->visit(this); }

// Estructuras compuestas
int Body::accept(Visitor *visitor) { return visitor->visit(this); }
int Program::accept(Visitor *visitor) { return visitor->visit(this); }

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 2: IMPLEMENTACIÓN DE PrintVisitor
///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BoolExp *exp) {
  if (exp->valor != 0) {
    cout << "true";
  } else {
    cout << "false";
  }
  return 0;
}

int PrintVisitor::visit(NotExp *exp) {
  cout << "not ";
  exp->e->accept(this);
  return 0;
}

int PrintVisitor::visit(NegExp *exp) {
  cout << "- ";
  exp->e->accept(this);
  return 0;
}

int PrintVisitor::visit(NumberExp *exp) {
  cout << exp->value;
  return 0;
}

int PrintVisitor::visit(IdExp *exp) {
  cout << exp->value;
  return 0;
}

int PrintVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  cout << ' ' << Exp::binopToChar(exp->op) << ' ';
  exp->right->accept(this);
  return 0;
}

int PrintVisitor::visit(PrintStm *stm) {
  cout << "print(";
  stm->e->accept(this);
  cout << ")" << endl;
  return 0;
}

int PrintVisitor::visit(IfStm *stm) {
  cout << "if ";
  stm->cond->accept(this);
  cout << "then" << endl;
  stm->trueB->accept(this);
  if (stm->elseB != nullptr) {
    stm->elseB->accept(this);
  }
  cout << "endif" << endl;
  return 0;
}

int PrintVisitor::visit(AssignStm *stm) {
  cout << stm->id << "=";
  stm->e->accept(this);
  cout << endl;
  return 0;
}

int PrintVisitor::visit(ReturnStm *stm) {
  cout << "return ";
  if (stm->e)
    stm->e->accept(this);
  cout << endl;
  return 0;
}

int PrintVisitor::visit(VarDec *vd) {
  cout << "var " << vd->tipo;
  for (auto i : vd->variables) {
    cout << i << ",";
  }
  cout << ";" << endl;
  return 0;
}

int PrintVisitor::visit(Body *b) {
  cout << "{\n";
  for (auto i : b->vdlist) {
    i->accept(this);
  }
  for (auto i : b->stmlist) {
    i->accept(this);
  }
  cout << "}\n";
  return 0;
}

int PrintVisitor::visit(FcallExp *fcall) {
  cout << fcall->nombre << "(";
  for (auto i : fcall->argumentos) {
    i->accept(this);
    cout << ",";
  }
  cout << ")";
  return 0;
}

int PrintVisitor::visit(FunDec *fd) {
  cout << "fun " << fd->tipo << " " << fd->nombre << "(";
  for (size_t i = 0; i < fd->Nparametros.size(); ++i) {
    cout << fd->Tparametros[i];
    if (i < fd->Nparametros.size() - 1)
      cout << ", ";
  }
  cout << ") ";
  fd->cuerpo->accept(this);
  cout << endl;
  return 0;
}

int PrintVisitor::visit(Program *p) {
  for (auto d : p->vdlist)
    d->accept(this);
  for (auto d : p->fdlist)
    d->accept(this);
  return 0;
}

void PrintVisitor::imprimir(Program *programa) {
  if (programa) {
    programa->accept(this);
  }
}

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 3: IMPLEMENTACIÓN DE EVALVisitor
///////////////////////////////////////////////////////////////////////////////////

int EVALVisitor::visit(BoolExp *exp) { return exp->valor; }

int EVALVisitor::visit(NotExp *exp) { return not exp->e->accept(this); }
int EVALVisitor::visit(NegExp *exp) { return -exp->e->accept(this); }

int EVALVisitor::visit(NumberExp *exp) { return exp->value; }

int EVALVisitor::visit(IdExp *exp) { return env.lookup(exp->value); }

int EVALVisitor::visit(BinaryExp *exp) {
  int v1 = exp->left->accept(this);
  int v2 = exp->right->accept(this);
  int result = 0;

  switch (exp->op) {
  case PLUS_OP:
    result = v1 + v2;
    break;
  case MINUS_OP:
    result = v1 - v2;
    break;
  case MUL_OP:
    result = v1 * v2;
    break;
  case DIV_OP:
    if (v2 != 0)
      result = v1 / v2;
    else {
      cout << "Error: división por cero" << endl;
      result = 0;
    }
    break;
  case POW_OP:
    result = pow(v1, v2);
    break;
  case LE_OP:
    result = v1 < v2;
    break;
  case AND_OP:
    result = v1 and v2;
    break;
  default:
    cout << "Operador desconocido" << endl;
    result = 0;
  }
  return result;
}

int EVALVisitor::visit(PrintStm *p) {
  cout << p->e->accept(this) << endl;
  return 0;
}

int EVALVisitor::visit(IfStm *s) {
  if (s->cond->accept(this)) {
    s->trueB->accept(this);
  } else {
    if (s->elseB != nullptr) {
      s->elseB->accept(this);
    }
  }
  return 0;
}

int EVALVisitor::visit(AssignStm *p) {
  env.update(p->id, p->e->accept(this));
  return 0;
}

int EVALVisitor::visit(ReturnStm *stm) {
  retcall = true;
  retval = stm->e->accept(this);
  return 0;
}
int EVALVisitor::visit(VarDec *vd) {
  for (auto &i : vd->variables) {
    env.add_var(i); // solo declara, valor por defecto = 0
  }
  return 0;
}

int EVALVisitor::visit(Body *b) {
  env.add_level();
  for (auto i : b->vdlist) {
    i->accept(this);
  }
  for (auto i : b->stmlist) {
    i->accept(this);
  }
  env.remove_level();
  return 0;
}

int EVALVisitor::visit(FcallExp *fcall) {
  retcall = false;
  vector<int> arg;
  for (auto i : fcall->argumentos) {
    arg.push_back(i->accept(this));
  }
  FunDec *fd = envfun[fcall->nombre];
  env.add_level();

  // Cargar los parámetros con sus valores
  for (size_t i = 0; i < arg.size(); ++i) {
    env.add_var(fd->Nparametros[i], arg[i]);
  }

  fd->cuerpo->accept(this);
  env.remove_level();

  if (retcall) {
    return retval;
  } else {
    cout << "Error: la función '" << fcall->nombre << "' no tiene retorno"
         << endl;
    exit(0);
  }
}

int EVALVisitor::visit(FunDec *fd) {
  envfun[fd->nombre] = fd;
  return 0;
}

int EVALVisitor::visit(Program *p) {
  env.add_level();
  for (auto i : p->vdlist) {
    i->accept(this);
  }
  for (auto i : p->fdlist) {
    i->accept(this);
  }
  if (envfun.count("main")) {
    envfun["main"]->cuerpo->accept(this);
  } else {
    cout << "no existe main" << endl;
    exit(0);
  }
  env.remove_level();
  return 0;
}

void EVALVisitor::interprete(Program *programa) {
  if (programa) {
    cout << "Interprete:" << endl;
    programa->accept(this);
  }
}
