#include "ast.hpp"
#include "lexer.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace GTOML {
    typedef std::shared_ptr<TOMLNode> Node;
    class Parser {
        public:
            std::vector<Node> parsedNodes;

            Parser(std::string file_path) : lexer(file_path) {
                lexer.read();
                lexer.lex();
                lexer.toknizer();
                Parse();
            };
            bool Parse();

            void printIR();
            void printNodeIR(const Node& node, int indentLevel);

            std::string getValueByKey(const std::string& key);
            std::string getTableValue(const std::string& key);
            std::vector<std::string> getArrayValue(const std::string& tableAndKey);

            std::vector<std::string> ArrayToVector(const Node& node);
        private:
            Lexer lexer;
            std::string file_path;

            bool expect(Token token);
            void consume();

            Node parseKey();
            Node parseArray();
            Node parseTable();


            Node getNodeByKey(const std::string& key);
    };
}  // namespace GTOML
