#include "parser.hpp"

using namespace GTOML;

bool Parser::Parse() {
  Token currentTokenType = lexer.GetCurrentToken().type;

  while (currentTokenType != Token::EoF) {
    if (currentTokenType == Token::IDENTIFIER) {
      parseKey();
    } else {
      std::cerr << "Unexpected token: " << lexer.ToString(currentTokenType)
                << std::endl;
      return false;
    }
    currentTokenType = lexer.GetCurrentToken().type;
  }
  return true;
}

bool Parser::expect(Token token) {
  Token currentToken = lexer.GetCurrentToken().type;
  if (currentToken != token) {
    std::cerr << "Expected " << lexer.ToString(token) << " but got "
              << lexer.ToString(currentToken) << std::endl;
    return false;
  }
  return true;
}

void Parser::consume() {
  if (lexer.hasMoreTokens()) {
    lexer.currentTokenIndex++;
  }
}

std::shared_ptr<TOMLNode> Parser::parseKey() {
  expect(Token::IDENTIFIER);
  std::string key = lexer.GetCurrentToken().value;
  consume();
  expect(Token::EQUAL);
  consume();

  std::shared_ptr<TOMLNode> keyValueNode;

  if (lexer.GetCurrentToken().type == Token::LEFT_BRACKET) {
    keyValueNode = parseArray();
  } else {
    Token currentToken = lexer.GetCurrentToken().type;

    if (currentToken == Token::STRING) {
      std::string value = lexer.GetCurrentToken().value;
      value = value.substr(1, value.size() - 2);
      consume();
      keyValueNode = std::make_shared<KeyValueNode>(
          key, std::make_shared<StringNode>(value));
    } else if (currentToken == Token::NUMBER) {
      int intValue = std::stoi(lexer.GetCurrentToken().value);
      consume();
      keyValueNode = std::make_shared<KeyValueNode>(
          key, std::make_shared<IntegerNode>(intValue));
    } else if (currentToken == Token::FLOAT) {
      double floatValue = std::stod(lexer.GetCurrentToken().value);
      consume();

      std::ostringstream stream;
      stream << std::fixed << std::setprecision(1) << floatValue;
      keyValueNode = std::make_shared<KeyValueNode>(
          key, std::make_shared<FloatNode>(stream.str()));

    } else if (currentToken == Token::BOOL) {
      bool boolValue;
      if (lexer.GetCurrentToken().value == "true") {
        boolValue = true;
      } else {
        boolValue = false;
      }
      consume();
      keyValueNode = std::make_shared<KeyValueNode>(
          key, std::make_shared<BoolNode>(boolValue));
    } else {
      std::cerr << "Unexpected token: " << lexer.ToString(currentToken)
                << std::endl;
      return nullptr;
    }
  }

  parsedNodes.push_back(keyValueNode);

  return keyValueNode;
}

std::shared_ptr<TOMLNode> Parser::parseArray() {
  std::string arrayName = lexer.PervPrevToken().value;
  expect(Token::LEFT_BRACKET);
  consume();

  std::vector<std::shared_ptr<TOMLNode>> elements;

  while (lexer.GetCurrentToken().type != Token::RIGHT_BRACKET) {
    Token currentToken = lexer.GetCurrentToken().type;
    std::shared_ptr<TOMLNode> arrayElement;

    if (currentToken == Token::STRING) {
      std::string value = lexer.GetCurrentToken().value;
      value = value.substr(1, value.size() - 1);
      if (!value.empty()) {
        arrayElement = std::make_shared<StringNode>(value);
        elements.push_back(arrayElement);
      }
      consume();
    } else if (currentToken == Token::FLOAT) {
      double floatValue = std::stod(lexer.GetCurrentToken().value);
      std::ostringstream stream;
      stream << std::fixed << std::setprecision(2) << floatValue;
      arrayElement = std::make_shared<FloatNode>(stream.str());
      elements.push_back(arrayElement);
      consume();
    } else if (currentToken == Token::NUMBER) {
      int intValue = std::stoi(lexer.GetCurrentToken().value);
      arrayElement = std::make_shared<IntegerNode>(intValue);
      elements.push_back(arrayElement);
      consume();
    } else if (currentToken == Token::BOOL) {
      bool boolValue;
      if (lexer.GetCurrentToken().value == "true") {
        boolValue = true;
      } else {
        boolValue = false;
      }
      arrayElement = std::make_shared<BoolNode>(boolValue);
      elements.push_back(arrayElement);
      consume();
    } else {
      std::cerr << "Unexpected token in array: " << lexer.ToString(currentToken)
                << std::endl;
      return nullptr;
    }

    while (lexer.GetCurrentToken().type == Token::COMMA) {
      consume();
    }
  }

  expect(Token::RIGHT_BRACKET);
  consume();

  return std::make_shared<ArrayNode>(arrayName, elements);
}

void Parser::printIR() {
  std::cout << "IR:     "
            << "Size: " << parsedNodes.size() << std::endl;
  for (const auto& node : parsedNodes) {
    printNodeIR(node);
  }
}

void Parser::printNodeIR(const std::shared_ptr<TOMLNode>& node) {
  if (auto keyValueNode = std::dynamic_pointer_cast<KeyValueNode>(node)) {
    std::cout << "Key: " << keyValueNode->key << ", Value: ";
    printNodeIR(keyValueNode->value);
  } else if (auto stringNode = std::dynamic_pointer_cast<StringNode>(node)) {
    std::cout << "String: " << stringNode->value << std::endl;
  } else if (auto integerNode = std::dynamic_pointer_cast<IntegerNode>(node)) {
    std::cout << "Integer: " << integerNode->value << std::endl;
  } else if (auto floatNode = std::dynamic_pointer_cast<FloatNode>(node)) {
    std::cout << "Float: " << std::fixed << std::setprecision(6)
              << floatNode->value << std::endl;
  } else if (auto boolNode = std::dynamic_pointer_cast<BoolNode>(node)) {
    std::cout << "Bool: " << (boolNode->value ? "true" : "false") << std::endl;
  } else if (auto arrayNode = std::dynamic_pointer_cast<ArrayNode>(node)) {
    std::cout << "Array: " << arrayNode->array_name << std::endl;
    for (const auto& element : arrayNode->elements) {
      std::cout << "  ";
      printNodeIR(element);
    }
  } else if (auto tableNode = std::dynamic_pointer_cast<TableNode>(node)) {
    std::cout << "Table:" << std::endl;
    for (const auto& entry : tableNode->entries) {
      printNodeIR(entry);
    }
  }
}

std::string Parser::getValueByKey(const std::string& key) {
  for (const auto& node : parsedNodes) {
    if (auto arrayNode = std::dynamic_pointer_cast<ArrayNode>(node)) {
      if (arrayNode->array_name == key) {
        std::string arrayValue = "[";

        for (size_t i = 0; i < arrayNode->elements.size(); ++i) {
          const auto& element = arrayNode->elements[i];
          if (auto stringElement =
                  std::dynamic_pointer_cast<StringNode>(element)) {
            arrayValue += stringElement->value;
          } else if (auto intElement =
                         std::dynamic_pointer_cast<IntegerNode>(element)) {
            arrayValue += std::to_string(intElement->value);
          } else if (auto floatElement =
                         std::dynamic_pointer_cast<FloatNode>(element)) {
            arrayValue += floatElement->value;
          }

          if (i != arrayNode->elements.size() - 1) {
            arrayValue += ",";
          }
        }

        arrayValue += "]";

        return arrayValue;
      }
    } else if (auto keyValueNode =
                   std::dynamic_pointer_cast<KeyValueNode>(node)) {
      if (keyValueNode->key == key) {
        auto valueNode = keyValueNode->value;

        if (auto stringNode =
                std::dynamic_pointer_cast<StringNode>(valueNode)) {
          return stringNode->value;
        } else if (auto intNode =
                       std::dynamic_pointer_cast<IntegerNode>(valueNode)) {
          return std::to_string(intNode->value);
        } else if (auto floatNode =
                       std::dynamic_pointer_cast<FloatNode>(valueNode)) {
          return floatNode->value;
        } else if (auto boolNode =
                       std::dynamic_pointer_cast<BoolNode>(valueNode)) {
          return boolNode->value ? "true" : "false";
        }
      }
    }
  }
  return "ERROR: Could not find \"" + key + "\" in file " + getFilePath();
}

std::vector<std::string> Parser::ArrayToVector(
    const std::shared_ptr<TOMLNode>& node) {
  std::vector<std::string> vec;
  if (auto arrayNode = std::dynamic_pointer_cast<ArrayNode>(node)) {
    for (const auto& element : arrayNode->elements) {
      if (auto stringNode = std::dynamic_pointer_cast<StringNode>(element)) {
        vec.push_back(stringNode->value);
      } else if (auto intNode =
                     std::dynamic_pointer_cast<IntegerNode>(element)) {
        vec.push_back(std::to_string(intNode->value));
      } else if (auto floatNode =
                     std::dynamic_pointer_cast<FloatNode>(element)) {
        vec.push_back(floatNode->value);
      } else if (auto boolNode = std::dynamic_pointer_cast<BoolNode>(element)) {
        vec.push_back(boolNode->value ? "true" : "false");
      }
    }
  }
  return vec;
}

std::shared_ptr<TOMLNode> Parser::getNodeByKey(const std::string& key) {
  for (const auto& node : parsedNodes) {
    if (auto arrayNode = std::dynamic_pointer_cast<ArrayNode>(node)) {
      if (arrayNode->array_name == key) {
        return arrayNode;
      }
    } else if (auto keyValueNode =
                   std::dynamic_pointer_cast<KeyValueNode>(node)) {
      if (keyValueNode->key == key) {
        auto valueNode = keyValueNode->value;

        if (auto stringNode =
                std::dynamic_pointer_cast<StringNode>(valueNode)) {
          return stringNode;
        } else if (auto intNode =
                       std::dynamic_pointer_cast<IntegerNode>(valueNode)) {
          return intNode;
        } else if (auto floatNode =
                       std::dynamic_pointer_cast<FloatNode>(valueNode)) {
          return floatNode;
        } else if (auto boolNode =
                       std::dynamic_pointer_cast<BoolNode>(valueNode)) {
          return boolNode;
        }
      }
    }
  }
  return nullptr;
}
