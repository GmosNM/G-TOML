#include <iomanip>
#include <iostream>
#include <sstream>

#include "ast.hpp"
#include "lexer.hpp"

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
  void Parse();

  void printIR();
  void printNodeIR(const Node& node);

  std::string getFilePath() { return file_path; }
  std::string getValueByKey(const std::string& key);
  std::shared_ptr<TOMLNode> getNodeByKey(const std::string& key);

  std::vector<std::string> ArrayToVector(const Node& node);

 private:
  GTOML::Lexer lexer;
  std::string file_path;

  bool expect(Token token);
  void consume();

  std::shared_ptr<TOMLNode> parseKey();
  std::shared_ptr<TOMLNode> parseArray();
};
}  // namespace GTOML
