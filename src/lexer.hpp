#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace GTOML {
  enum class Token {
    LEFT_BRACKET,
    RIGHT_BRACKET,
    EQUAL,
    IDENTIFIER,
    NUMBER,
    STRING,
    FLOAT,
    BOOL,
    DOT,
    COMMA,
    EoF,
  };

  struct SToken {
    Token type;
    std::string value;
    Token next;
    Token prev;
    std::string ToString(){
      switch (type) {
        case Token::LEFT_BRACKET:
        return "[";
          break;
        case Token::RIGHT_BRACKET:
          return "]";
          break;
        case Token::EQUAL:
          return "=";
          break;
        case Token::IDENTIFIER:
          return "IDENTIFIER";
          break;
        case Token::NUMBER:
          return "NUMBER";
          break;
        case Token::STRING:
          return "STRING";
          break;
        case Token::FLOAT:
          return "FLOAT";
          break;
        case Token::BOOL:
          return "BOOL";
          break;
        case Token::DOT:
          return ".";
          break;
        case Token::COMMA:
          return ",";
          break;
        case Token::EoF:
          return "EOF";
          break;
      }
    }
  };

  class Lexer {
   public:
    Lexer(std::string filename) : filename(filename){
    };
    size_t currentTokenIndex = 0;

    SToken NextToken();
    SToken GetCurrentToken();
    SToken PrevToken();
    SToken PervPrevToken();

    void read();
    void print_file();
    void toknizer();
    void lex();
    void print_token_types(int indentLevel);
    void print_tokens();
    void print_token(const SToken& token, int indentLevel,
                     const std::string& tokenTypeStr);
    void print_tokens_type();
    Token classify_token(const SToken& token);
    bool is_number(const std::string& str);
    bool is_string(const std::string& str);
    bool hasMoreTokens();
    SToken CreateEmptyToken();
    SToken CreateEofToken();

    std::string getFilePath() { return filename; }


    std::string ToString(Token token);
   private:
    std::string filename;
    std::string content;
    std::vector<SToken> tokens;
    SToken currentToken;
    std::vector<Token> tokenTypes;
  };
} // namespace GTOML

