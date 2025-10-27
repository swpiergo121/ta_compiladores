#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "token.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner *sc) : scanner(sc) {
  previous = nullptr;
  current = scanner->nextToken();
  if (current->type == Token::ERR) {
    throw runtime_error("Error léxico");
  }
}

bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype) {
  if (isAtEnd())
    return false;
  return current->type == ttype;
}

bool Parser::advance() {
  if (!isAtEnd()) {
    Token *temp = current;
    if (previous)
      delete previous;
    current = scanner->nextToken();
    previous = temp;

    if (check(Token::ERR)) {
      throw runtime_error("Error lexico");
    }
    return true;
  }
  return false;
}

bool Parser::isAtEnd() { return (current->type == Token::END); }

// =============================
// Reglas gramaticales
// =============================

Program *Parser::parseProgram() {
  Program *p = new Program();

  if (check(Token::VAR)) {
    p->vdlist.push_back(parseVarDec());
    match(Token::SEMICOL);
    while (check(Token::VAR)) {
      p->vdlist.push_back(parseVarDec());
    }
  }

  if (check(Token::FUN)) {
    p->fdlist.push_back(parseFunDec());
    while (check(Token::FUN)) {
      p->fdlist.push_back(parseFunDec());
    }
  }

  if (!isAtEnd()) {
    throw runtime_error("Error sintáctico");
  }

  cout << "Parseo exitoso" << endl;
  return p;
}

VarDec *Parser::parseVarDec() {
  VarDec *vd = new VarDec();
  match(Token::VAR);
  match(Token::ID);
  vd->tipo = previous->text;
  match(Token::ID);
  vd->variables.push_back(previous->text);
  while (match(Token::COMA)) {
    vd->variables.push_back(previous->text);
  }
  return vd;
}

FunDec *Parser::parseFunDec() {
  FunDec *fd = new FunDec();
  match(Token::FUN);
  match(Token::ID);
  fd->tipo = previous->text;
  match(Token::ID);
  fd->nombre = previous->text;
  match(Token::LPAREN);
  if (match(Token::ID)) {
    fd->Tparametros.push_back(previous->text);
    match(Token::ID);
    fd->Nparametros.push_back(previous->text);
    while (match(Token::COMA)) {
      match(Token::ID);
      fd->Tparametros.push_back(previous->text);
      match(Token::ID);
      fd->Nparametros.push_back(previous->text);
    }
  }
  match(Token::RPAREN);
  fd->cuerpo = parseBody();
  match(Token::ENDFUN);
  return fd;
}

Body *Parser::parseBody() {
  Body *b = new Body();
  if (check(Token::VAR)) {
    b->vdlist.push_back(parseVarDec());
    match(Token::SEMICOL);
    while (check(Token::VAR)) {
      b->vdlist.push_back(parseVarDec());
    }
  }
  b->stmlist.push_back(parseStm());
  while (match(Token::SEMICOL)) {
    b->stmlist.push_back(parseStm());
  }
  return b;
}

Stm *Parser::parseStm() {
  Stm *a;
  Exp *e;
  string variable;
  if (match(Token::ID)) {
    variable = previous->text;
    match(Token::ASSIGN);
    e = parseCE();
    return new AssignStm(variable, e);
  } else if (match(Token::IF)) {
    cout << "Starting IF " << endl;
    e = parseCE();
    match(Token::THEN);
    Body *b1 = parseBody();
    cout << "After first body " << endl;
    Body *b2 = nullptr;
    if (match(Token::ELSE)) {
      b2 = parseBody();
    }
    cout << "After second body " << endl;
    match(Token::ENDIF);
    return new IfStm(e, b1, b2);
  } else if (match(Token::PRINT)) {
    match(Token::LPAREN);
    e = parseCE();
    match(Token::RPAREN);
    return new PrintStm(e);
  } else if (match(Token::RETURN)) {
    ReturnStm *r = new ReturnStm();
    match(Token::LPAREN);
    r->e = parseCE();
    match(Token::RPAREN);
    return r;
  } else {
    throw runtime_error("Error sintáctico");
  }
  return a;
}
Exp *Parser::parseCE() {
  bool is_not = false;
  if (match(Token::NOT)) {
    is_not = true;
  }

  Exp *l = parseAE();
  if (match(Token::AND)) {
    BinaryOp op;
    op = AND_OP;
    Exp *r = parseAE();
    l = new BinaryExp(l, r, op);
  }

  if (is_not) {
    Exp *e = nullptr;
    e = new NotExp(l);
    l = e;
  }
  return l;
}

Exp *Parser::parseAE() {
  bool is_neg = false;
  if (match(Token::MINUS)) {
    is_neg = true;
  }

  Exp *l = parseBE();
  if (match(Token::LE)) {
    BinaryOp op;
    op = LE_OP;
    Exp *r = parseBE();
    l = new BinaryExp(l, r, op);
  }

  if (is_neg) {
    Exp *e = nullptr;
    e = new NegExp(l);
    l = e;
  }
  return l;
}

Exp *Parser::parseBE() {
  Exp *l = parseE();
  while (match(Token::PLUS) || match(Token::MINUS)) {
    BinaryOp op;
    if (previous->type == Token::PLUS) {
      op = PLUS_OP;
    } else {
      op = MINUS_OP;
    }
    Exp *r = parseE();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseE() {
  Exp *l = parseT();
  while (match(Token::MUL) || match(Token::DIV)) {
    BinaryOp op;
    if (previous->type == Token::MUL) {
      op = MUL_OP;
    } else {
      op = DIV_OP;
    }
    Exp *r = parseT();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseT() {
  Exp *l = parseF();
  if (match(Token::POW)) {
    BinaryOp op = POW_OP;
    Exp *r = parseF();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseF() {
  Exp *e;
  if (match(Token::NUM)) {
    return new NumberExp(stoi(previous->text));
  } else if (match(Token::TRUE)) {
    BoolExp *e = new BoolExp();
    e->valor = 1;
    return e;
  } else if (match(Token::FALSE)) {
    BoolExp *e = new BoolExp();
    e->valor = 0;
    return e;
  }

  else if (match(Token::LPAREN)) {
    e = parseCE();
    match(Token::RPAREN);
    return e;
  } else if (match(Token::ID)) {
    string nom = previous->text;
    if (match(Token::LPAREN)) {
      FcallExp *fcall = new FcallExp();
      fcall->nombre = nom, fcall->argumentos.push_back(parseCE());
      while (match(Token::COMA)) {
        fcall->argumentos.push_back(parseCE());
      }
      match(Token::RPAREN);
      return fcall;
    } else {
      return new IdExp(nom);
    }
  }

  else {
    throw runtime_error("Error sintáctico");
  }
}
