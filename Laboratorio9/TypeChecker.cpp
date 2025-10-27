#include "TypeChecker.h"
#include <iostream>
#include <stdexcept>

// TODO: Implement while statement and if statement
// TODO: Implement unary operators, like not and -
// TODO: Implement more logical operators; like or, xor
// TODO: Implement more relational operators; like >=, <=, ==
// DONE:
// Make sure the int and bool function have a return
// Make sure a main function exist
// The void functions musn't have a return statement
// Match the return type of the function with the type of the return statement
// Match number and type of parameters of function with the arguments of
// fCallExp

using namespace std;

Type *NumberExp::accept(TypeVisitor *v) { return v->visit(this); }
Type *IdExp::accept(TypeVisitor *v) { return v->visit(this); }
Type *BinaryExp::accept(TypeVisitor *v) { return v->visit(this); }
Type *FcallExp::accept(TypeVisitor *v) { return v->visit(this); }
Type *BoolExp::accept(TypeVisitor *v) { return v->visit(this); }
Type *NotExp::accept(TypeVisitor *v) { return v->visit(this); }
Type *NegExp::accept(TypeVisitor *v) { return v->visit(this); }

void AssignStm::accept(TypeVisitor *v) { v->visit(this); }
void IfStm::accept(TypeVisitor *v) { v->visit(this); }
void PrintStm::accept(TypeVisitor *v) { v->visit(this); }
void ReturnStm::accept(TypeVisitor *v) { v->visit(this); }

void VarDec::accept(TypeVisitor *v) { v->visit(this); }
void FunDec::accept(TypeVisitor *v) { v->visit(this); }
void Body::accept(TypeVisitor *v) { v->visit(this); }
void Program::accept(TypeVisitor *v) { v->visit(this); }

// ===========================================================
//   Constructor del TypeChecker
// ===========================================================

TypeChecker::TypeChecker() {
  intType = new Type(Type::INT);
  boolType = new Type(Type::BOOL);
  voidType = new Type(Type::VOID);
}

// ===========================================================
//   Registrar funciones globales
// ===========================================================

void TypeChecker::add_function(FunDec *fd) {
  if (functions.find(fd->nombre) != functions.end()) {
    cerr << "Error: función '" << fd->nombre << "' ya fue declarada." << endl;
    exit(0);
  }

  Type *returnType = new Type();
  if (!returnType->set_basic_type(fd->tipo)) {
    cerr << "Error: tipo de retorno no válido en función '" << fd->nombre
         << "'." << endl;
    exit(0);
  }

  functions[fd->nombre] = returnType;
}

// ===========================================================
//   Método principal de verificación
// ===========================================================

void TypeChecker::typecheck(Program *program) {
  if (program)
    program->accept(this);
  cout << "Revisión exitosa" << endl;
}

// ===========================================================
//   Nivel superior: Programa y Bloque
// ===========================================================

void TypeChecker::visit(Program *p) {
  // Primero registrar funciones
  for (auto f : p->fdlist)
    add_function(f);

  auto it = functions.find("main");
  if (it == functions.end()) {
    cerr << "Error: No existe funcion main." << endl;
    exit(0);
  }

  env.add_level();
  for (auto v : p->vdlist)
    v->accept(this);
  for (auto f : p->fdlist)
    f->accept(this);
  env.remove_level();
}

void TypeChecker::visit(Body *b) {
  env.add_level();
  for (auto v : b->vdlist)
    v->accept(this);
  for (auto s : b->stmlist)
    s->accept(this);
  env.remove_level();
}

// ===========================================================
//   Declaraciones
// ===========================================================

void TypeChecker::visit(VarDec *v) {
  Type *t = new Type();
  if (!t->set_basic_type(v->tipo)) {
    cerr << "Error: tipo de variable no válido." << endl;
    exit(0);
  }

  for (const auto &id : v->variables) {
    if (env.check(id)) {
      cerr << "Error: variable '" << id << "' ya declarada." << endl;
      exit(0);
    }
    env.add_var(id, t);
  }
}

void TypeChecker::visit(FunDec *f) {
  env.add_level();
  vector<Type *> parametersTypes;
  for (size_t i = 0; i < f->Nparametros.size(); ++i) {
    Type *pt = new Type();
    if (!pt->set_basic_type(f->Tparametros[i])) {
      cerr << "Error: tipo de parámetro inválido en función '" << f->nombre
           << "'." << endl;
      exit(0);
    }
    env.add_var(f->Nparametros[i], pt);
    parametersTypes.push_back(pt);
  }

  typeFunction = new Type();
  typeFunction->set_basic_type(f->tipo);
  insideFunction = true;

  f->cuerpo->accept(this);

  if (!returnPassed &&
      (typeFunction->match(intType) || typeFunction->match(boolType))) {
    cerr << "Error: En la función '" << f->nombre
         << "' no hay un return statement." << endl;
    exit(0);
  }
  returnPassed = false;

  functionsNParameters[f->nombre] = f->Nparametros.size();
  functionsParameters[f->nombre] = parametersTypes;
  env.remove_level();

  insideFunction = false;
  delete typeFunction;
}

// ===========================================================
//   Sentencias
// ===========================================================

void TypeChecker::visit(PrintStm *stm) {
  Type *t = stm->e->accept(this);
  if (!(t->match(intType) || t->match(boolType))) {
    cerr << "Error: tipo inválido en print (solo int o bool)." << endl;
    exit(0);
  }
}

void TypeChecker::visit(AssignStm *stm) {
  if (!env.check(stm->id)) {
    cerr << "Error: variable '" << stm->id << "' no declarada." << endl;
    exit(0);
  }

  Type *varType = env.lookup(stm->id);
  Type *expType = stm->e->accept(this);

  if (!varType->match(expType)) {
    cerr << "Error: tipos incompatibles en asignación a '" << stm->id << "'."
         << endl;
    exit(0);
  }
}

void TypeChecker::visit(IfStm *stm) {
  Type *condType = stm->cond->accept(this);
  if (!condType->match(boolType)) {
    cerr << "Error: la condicional de un if debe ser tipo bool." << endl;
    exit(0);
  }
}

void TypeChecker::visit(ReturnStm *stm) {
  if (stm->e) {
    Type *t = stm->e->accept(this);
    if (!typeFunction->match(t)) {
      cerr << "Error: tipo de regreso diferente al de la funcion." << endl;
      exit(0);
    }
    if (typeFunction->match(voidType)) {
      cerr << "Error: regreso para una funcion de tipo void." << endl;
      exit(0);
    }

    if (!(t->match(intType) || t->match(boolType) || t->match(voidType))) {
      cerr << "Error: tipo inválido en return." << endl;
      exit(0);
    }
    returnPassed = true;
  }
}

// ===========================================================
//   Expresiones
// ===========================================================

Type *TypeChecker::visit(BinaryExp *e) {
  Type *left = e->left->accept(this);
  Type *right = e->right->accept(this);

  switch (e->op) {
  case PLUS_OP:
  case MINUS_OP:
  case MUL_OP:
  case DIV_OP:
  case POW_OP:
    if (!(left->match(intType) && right->match(intType))) {
      cerr << "Error: operación aritmética requiere operandos int." << endl;
      exit(0);
    }
    return intType;
  case LE_OP:
    if (!(left->match(intType) && right->match(intType))) {
      cerr << "Error: operación de comparacion requiere operandos int." << endl;
      exit(0);
    }
    return boolType;
  case AND_OP:
    if (!(left->match(boolType) && right->match(boolType))) {
      cerr << "Error: operación logica requiere operandos bool." << endl;
      exit(0);
    }
    return boolType;

  default:
    cerr << "Error: operador binario no soportado." << endl;
    exit(0);
  }
}

Type *TypeChecker::visit(NumberExp *e) { return intType; }

Type *TypeChecker::visit(BoolExp *e) { return boolType; }

Type *TypeChecker::visit(NotExp *e) {
  Type *te = e->e->accept(this);
  if (!te->match(boolType)) {
    cerr << "Error: not can only be used with booleans." << endl;
    exit(0);
  }
  return te;
}

Type *TypeChecker::visit(NegExp *e) {
  Type *te = e->e->accept(this);
  if (!te->match(intType)) {
    cerr << "Error: - unary operator can only be used with integers." << endl;
    exit(0);
  }
  return te;
}

Type *TypeChecker::visit(IdExp *e) {
  if (!env.check(e->value)) {
    cerr << "Error: variable '" << e->value << "' no declarada." << endl;
    exit(0);
  }
  return env.lookup(e->value);
}

Type *TypeChecker::visit(FcallExp *e) {
  auto it = functions.find(e->nombre);
  if (it == functions.end()) {
    cerr << "Error: llamada a función no declarada '" << e->nombre << "'."
         << endl;
    exit(0);
  }

  return it->second;
  int numberParameters = functionsNParameters[e->nombre];
  int numberArguments = e->argumentos.size();
  if (numberArguments != numberParameters) {
    cerr << "Error: cantidad de argumentos es diferente a la de los parametros"
         << endl;
    exit(0);
  }
  auto iter = e->argumentos.begin();
  for (int i = 0; i < numberParameters; i++) {
    Type *parameterType = functionsParameters[e->nombre][i];
    Type *argumentType = (*iter)->accept(this);
    if (argumentType->match(parameterType)) {
      cerr << "Error: Atributo " << i
           << " no es del mismo tipo que el parametro" << endl;
      exit(0);
    }

    iter++;
  }
}
