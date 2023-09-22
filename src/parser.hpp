#include "ast.hpp"
#include "lexer.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace GTOML {
    typedef std::shared_ptr<TOMLNode> Node;
    class Parser {
        public:

            Parser(std::string file_path) : lexer(file_path) {
                file_path = file_path.substr(0, file_path.find_last_of('.'));
                lexer.read();
                lexer.lex();
                lexer.toknizer();
                Parse();
            };
            bool Parse();

            void printIR();


            std::string getValueByKey(const std::string& key);
            std::string getTableValue(const std::string& key);

        private:
            Lexer lexer;
            std::string file_path;
            std::vector<Node> parsedNodes;

            bool expect(Token token);
            void consume();

            Node parseKey();
            Node parseArray();
            Node parseTable();
            Node parseTableKey();
            Node parseTableArray();
            Node getNode(const std::string& key);


            void printValueNodeIR(const Node& node, int indentLevel);
            void printNodeIR(const Node& node, int indentLevel);


    };
}  // namespace GTOML
