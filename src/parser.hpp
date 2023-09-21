#include "ast.hpp"
#include "lexer.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace GTOML {
    class Parser {
        public:
            std::vector<std::shared_ptr<TOMLNode>> parsedNodes;

            Parser(GTOML::Lexer lexer)
                : lexer(lexer){
                    file_path = lexer.getFilePath();
                };
            void Parse();

            void printIR();
            void printNodeIR(const std::shared_ptr<TOMLNode>& node);

            std::string getFilePath() { return file_path; }
            std::string findValueByKey(const std::string& key);
        private:
            GTOML::Lexer lexer;
            std::string file_path;

            bool expect(Token token);
            void consume();

            std::shared_ptr<TOMLNode> parseKey();
            std::shared_ptr<TOMLNode> parseArray();
    };
}  // namespace GTOML
