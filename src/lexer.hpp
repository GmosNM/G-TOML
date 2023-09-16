#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum class Token {
  LEFT_BRACKET,
  RIGHT_BRACKET,
  EQUAL,
  IDENTIFIER,
  NUMBER,
  STRING,
  BOOL,
  EoF,
};

struct SToken {
  Token type;
  std::string value;
  Token next;
  Token prev;
};

class Lexer {
 public:
  Lexer(std::string filename) : filename(filename){};

  SToken NextToken();
  SToken GetCurrentToken();
  SToken PervToken();

  void read();
  void print_file();
  void toknizer();
  void lex();
  void print_token_types(int indentLevel);
  void print_tokens();
  void print_token(const SToken& token, int indentLevel,
                   const std::string& tokenTypeStr);
  Token classify_token(const SToken& token);
  bool is_number(const std::string& str);
  bool is_string(const std::string& str);

 private:
  std::string filename;
  std::string content;
  std::vector<SToken> tokens;
  SToken currentToken;
  std::vector<Token> tokenTypes;
  size_t currentTokenIndex = 0;
};
