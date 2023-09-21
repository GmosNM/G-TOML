#include "ast.hpp"
#include "lexer.hpp"

namespace GTOML {
class Parser {
 public:
  Parser(GTOML::Lexer lexer, std::string file)
      : lexer(lexer), file_path(file){};
  void Parse();

  void printIR();
  void printNodeIR(const std::shared_ptr<TOMLNode>& node);

 private:
  GTOML::Lexer lexer;
  std::string file_path;
  std::vector<std::shared_ptr<TOMLNode>> parsedNodes;

  bool expect(Token token);
  void consume();

  std::shared_ptr<TOMLNode> parseKey();
  std::shared_ptr<TOMLNode> parseArray();
};
}  // namespace GTOML
