#include <iostream>
#include "parser.hpp"

using namespace GTOML;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    GTOML::Lexer lexer(argv[1]);
    lexer.read();
    lexer.lex();
    lexer.toknizer();
    GTOML::Parser parser(lexer,argv[1]);

    parser.Parse();
    parser.printIR();

    return 0;
}

