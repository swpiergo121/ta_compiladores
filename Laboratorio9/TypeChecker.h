#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "ast.h"
#include "environment.h"
#include "semantic_types.h"
#include <string>
#include <unordered_map>

using namespace std;

class BinaryExp;
class NumberExp;
class Program;
class PrintStm;
class AssignStm;
class IfStm;
class FunDec;
class ReturnStm;
class Body;
class VarDec;
class FcallExp;
class BoolExp;
class NotExp;
class NegExp;

class TypeVisitor {
public:
  // --- Nodos de nivel superior ---
  virtual void visit(Program *p) = 0;
  virtual void visit(Body *b) = 0;
  virtual void visit(VarDec *v) = 0;
  virtual void visit(FunDec *f) = 0;

  // --- Sentencias ---
  virtual void visit(PrintStm *stm) = 0;
  virtual void visit(AssignStm *stm) = 0;
  virtual void visit(IfStm *stm) = 0;
  virtual void visit(ReturnStm *stm) = 0;

  // --- Expresiones ---
  virtual Type *visit(BinaryExp *e) = 0;
  virtual Type *visit(NumberExp *e) = 0;
  virtual Type *visit(IdExp *e) = 0;
  virtual Type *visit(BoolExp *e) = 0;
  virtual Type *visit(NotExp *e) = 0;
  virtual Type *visit(NegExp *e) = 0;
  virtual Type *visit(FcallExp *e) = 0;
};

// ──────────────────────────────────────────────
//   CLASE TYPECHECKER
// ──────────────────────────────────────────────

class TypeChecker : public TypeVisitor {
private:
  Environment<Type *> env;                 // Entorno de variables y sus tipos
  unordered_map<string, Type *> functions; // Entorno de funciones
  unordered_map<string, int>
      functionsNParameters; // Numero de parametros de una funcion
  unordered_map<string, vector<Type *>>
      functionsParameters; // Tipos de los parametros de una funcion
  Type *typeFunction;      // typo de funcion a al que se ha entrado el body
  bool insideFunction;     // Es verdad si el body le pertenece a una funcion
  bool returnPassed;       // Si es que en el body se ha pasado por una return

  // Tipos básicos
  Type *intType;
  Type *boolType;
  Type *voidType;

  // Registro de funciones
  void add_function(FunDec *fd);

public:
  TypeChecker();

  // Método principal de verificación
  void typecheck(Program *program);

  // --- Visitas de alto nivel ---
  void visit(Program *p) override;
  void visit(Body *b) override;
  void visit(VarDec *v) override;
  void visit(FunDec *f) override;

  // --- Sentencias ---
  void visit(PrintStm *stm) override;
  void visit(AssignStm *stm) override;
  void visit(IfStm *stm) override;
  void visit(ReturnStm *stm) override;

  // --- Expresiones ---
  Type *visit(BinaryExp *e) override;
  Type *visit(NumberExp *e) override;
  Type *visit(IdExp *e) override;
  Type *visit(FcallExp *e) override;
  Type *visit(BoolExp *e) override;
  Type *visit(NotExp *e) override;
  Type *visit(NegExp *e) override;
};

#endif // TYPECHECKER_H
