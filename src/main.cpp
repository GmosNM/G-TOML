#include <iostream>
#include "lexer.hpp"



int main(int argc, char *argv[]){
    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    Lexer lexer = Lexer(argv[1]);
    lexer.read();
    lexer.lex();
    lexer.toknizer();
    lexer.print_token_types(0);
    return 0;
}
