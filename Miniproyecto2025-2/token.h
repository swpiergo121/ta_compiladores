#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <string>

using namespace std;

class Token {
public:
  // Tipos de token
  enum Type {
    PLUS,     // +
    MINUS,    // -
    MUL,      // *
    DIV,      // /
    POW,      // **
    LPAREN,   // (
    RPAREN,   // )
    LBRACKET, // {
    RBRACKET, // }
    SQRT,     // sqrt
    NUM,      // Número
    ERR,      // Error
    ID,       // ID
    LE,
    FUN,
    ENDFUN,
    RETURN,
    SEMICOL,
    DOUBLEDOT, // :
    ASSIGN,
    PRINT,
    PRINTLN,
    IF,
    WHILE,
    DO,
    THEN,
    ENDIF,
    ENDWHILE,
    ELSE,
    FOR,
    END, // Fin de entrada
    VAR,
    VAL,
    COMA,
    TRUE,
    FALSE
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
