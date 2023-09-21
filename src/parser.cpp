#include "parser.hpp"

using namespace GTOML;

void Parser::Parse() {
  lexer.print_tokens_type();
  Token currentTokenType = lexer.GetCurrentToken().type;
  std::cout << "Current token type: " << lexer.ToString(currentTokenType)
            << std::endl;

  while (currentTokenType != Token::EoF) {
    if (currentTokenType == Token::IDENTIFIER) {
      parseKey();
    } else {
      std::cerr << "Unexpected token: " << lexer.ToString(currentTokenType)
                << std::endl;
      break;
    }
    currentTokenType = lexer.GetCurrentToken().type;
  }
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
      // Remove leading and trailing double quotes
      value = value.substr(1, value.size() - 1);
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
      keyValueNode = std::make_shared<KeyValueNode>(
          key, std::make_shared<FloatNode>(floatValue));
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
      arrayElement = std::make_shared<FloatNode>(floatValue);
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

  return std::make_shared<ArrayNode>(elements);
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
    std::cout << "Float: " << floatNode->value << std::endl;
  } else if (auto boolNode = std::dynamic_pointer_cast<BoolNode>(node)) {
    std::cout << "Bool: " << (boolNode->value ? "true" : "false") << std::endl;
  } else if (auto arrayNode = std::dynamic_pointer_cast<ArrayNode>(node)) {
    std::cout << "Array:" << std::endl;
    for (const auto& element : arrayNode->elements) {
      printNodeIR(element);
    }
  } else if (auto tableNode = std::dynamic_pointer_cast<TableNode>(node)) {
    std::cout << "Table:" << std::endl;
    for (const auto& entry : tableNode->entries) {
      printNodeIR(entry);
    }
  }
}
