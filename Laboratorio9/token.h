#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <string>

using namespace std;

class Token {
public:
  // Tipos de token
  enum Type {
    PLUS,   // +
    MINUS,  // -
    MUL,    // *
    DIV,    // /
    POW,    // **
    LPAREN, // (
    RPAREN, // )
    NUM,    // Número
    ERR,    // Error
    ID,     // ID
    FUN,
    TRUE,
    FALSE,
    ENDFUN,
    COMA,
    RETURN,
    VAR,
    LE,
    AND,
    SEMICOL,
    ASSIGN,
    PRINT,
    NOT,
    IF,
    ELSE,
    ENDIF,
    THEN,
    END // Fin de entrada
  };

  // Atributos
  Type type;
  string text;

  // Constructores
  Token(Type type);
  Token(Type type, char c);
  Token(Type type, const string &source, int first, int last);

  // Sobrecarga de operadores de salida
  friend ostream &operator<<(ostream &outs, const Token &tok);
  friend ostream &operator<<(ostream &outs, const Token *tok);
};

#endif // TOKEN_H
