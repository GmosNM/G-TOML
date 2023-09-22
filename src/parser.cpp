#include "parser.hpp"

using namespace GTOML;

bool Parser::Parse() {
  Token currentTokenType = lexer.GetCurrentToken().type;

  while (currentTokenType != Token::EoF) {
    if (currentTokenType == Token::IDENTIFIER) {
      parseKey();
    } else if (currentTokenType == Token::LEFT_BRACKET) {
      parseTable();
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
      if (keyValueNode->inside_table){
          return nullptr;
      }
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
  if (!keyValueNode->inside_table) {
    parsedNodes.push_back(keyValueNode);
  }

  return keyValueNode;
}


std::shared_ptr<TOMLNode> Parser::parseTable() {
    expect(Token::LEFT_BRACKET);
    consume();

    expect(Token::IDENTIFIER);
    std::string tableName = lexer.GetCurrentToken().value;
    consume();

    expect(Token::RIGHT_BRACKET);
    consume();

    std::vector<std::shared_ptr<TOMLNode>> entries;
    std::shared_ptr<TableNode> tableNode = std::make_shared<TableNode>(entries);
    tableNode->name = tableName;

    Token currentToken = lexer.GetCurrentToken().type;
    while (currentToken == Token::IDENTIFIER) {
        std::shared_ptr<TOMLNode> keyValueNode;

        if (currentToken == Token::IDENTIFIER) {
            keyValueNode = parseTableKey();
        } else if (currentToken == Token::LEFT_BRACKET) {
            keyValueNode = parseTableArray();
        }

        if (keyValueNode) {
            tableNode->entries.push_back(keyValueNode);
        } else {
            return nullptr;
        }
        currentToken = lexer.GetCurrentToken().type;
    }

    parsedNodes.push_back(tableNode);

    return tableNode;
}

std::shared_ptr<TOMLNode> Parser::parseTableArray() {
    expect(Token::LEFT_BRACKET);
    consume();

    expect(Token::IDENTIFIER);
    std::string tableName = lexer.GetCurrentToken().value;
    consume();

    expect(Token::RIGHT_BRACKET);
    consume();

    std::vector<std::shared_ptr<TOMLNode>> entries;

    Token currentToken = lexer.GetCurrentToken().type;
    while (currentToken == Token::LEFT_BRACKET) {
        std::shared_ptr<TOMLNode> tableNode = parseTable();
        if (tableNode) {
            entries.push_back(tableNode);
        } else {
            return nullptr;
        }
        currentToken = lexer.GetCurrentToken().type;
    }

    std::shared_ptr<ArrayNode> tableArrayNode = std::make_shared<ArrayNode>(tableName, entries);

    parsedNodes.push_back(tableArrayNode);

    return tableArrayNode;
}

Node Parser::parseTableKey() {
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
    for (const auto& node : parsedNodes) {
        printNodeIR(node, 0);
    }
}

void Parser::printNodeIR(const std::shared_ptr<TOMLNode>& node, int indentLevel) {
    std::string indent(indentLevel * 2, ' ');

    if (auto tableNode = std::dynamic_pointer_cast<TableNode>(node)) {
        std::cout << indent << "Table: " << tableNode->name << std::endl;
        for (const auto& entry : tableNode->entries) {
            printNodeIR(entry, indentLevel + 1);
        }
    } else if (auto arrayNode = std::dynamic_pointer_cast<ArrayNode>(node)) {
        std::cout << indent << "Array: " << arrayNode->array_name << std::endl;
        for (const auto& element : arrayNode->elements) {
            std::cout << "\t";
            printValueNodeIR(element, indentLevel + 1);
        }
    } else if (auto keyValueNode = std::dynamic_pointer_cast<KeyValueNode>(node)) {
        std::cout << indent << "Key: " << keyValueNode->key << ", Value: ";
        printValueNodeIR(keyValueNode->value, indentLevel);
    }
}


void Parser::printValueNodeIR(const std::shared_ptr<TOMLNode>& node, int indentLevel) {
  std::string indent(indentLevel * 2, ' ');

  if (auto stringNode = std::dynamic_pointer_cast<StringNode>(node)) {
    std::cout << "String: " << stringNode->value << std::endl;
  } else if (auto integerNode = std::dynamic_pointer_cast<IntegerNode>(node)) {
    std::cout << "Integer: " << integerNode->value << std::endl;
  } else if (auto floatNode = std::dynamic_pointer_cast<FloatNode>(node)) {
    std::cout << "Float: " << std::fixed << std::setprecision(6) << floatNode->value << std::endl;
  } else if (auto boolNode = std::dynamic_pointer_cast<BoolNode>(node)) {
    std::cout << "Bool: " << (boolNode->value ? "true" : "false") << std::endl;
  } else if (auto arrayNode = std::dynamic_pointer_cast<ArrayNode>(node)) {
    std::cout << "Array: " << arrayNode->array_name << std::endl;
    for (const auto& element : arrayNode->elements) {
      std::cout << indent << "  ";
      printValueNodeIR(element, indentLevel + 1);
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
  return "ERROR: Could not find \"" + key + "\" in file " + file_path;
}


std::string Parser::getTableValue(const std::string& tableAndKey) {
    size_t dotPos = tableAndKey.find('.');
    if (dotPos == std::string::npos) {
        return "ERROR: Invalid table.key format";
    }

    std::string tableName = tableAndKey.substr(0, dotPos);
    std::string key = tableAndKey.substr(dotPos + 1);

    for (const auto& node : parsedNodes) {
        if (auto tableNode = std::dynamic_pointer_cast<TableNode>(node)) {
            if (tableNode->name == tableName) {
                for (const auto& entry : tableNode->entries) {
                    if (auto keyValueNode =
                            std::dynamic_pointer_cast<KeyValueNode>(entry)) {
                        if (keyValueNode->key == key) {
                            if (auto arrayNode =
                                    std::dynamic_pointer_cast<ArrayNode>(
                                        keyValueNode->value)) {
                                if (!arrayNode->elements.empty()) {
                                    if (auto stringNode =
                                            std::dynamic_pointer_cast<StringNode>(
                                                arrayNode->elements[0])) {
                                        return stringNode->value;
                                    } else if (auto intNode =
                                            std::dynamic_pointer_cast<IntegerNode>(
                                                arrayNode->elements[0])) {
                                        return std::to_string(intNode->value);
                                    } else if (auto floatNode =
                                            std::dynamic_pointer_cast<FloatNode>(
                                                arrayNode->elements[0])) {
                                        return floatNode->value;
                                    } else if (auto boolNode =
                                            std::dynamic_pointer_cast<BoolNode>(
                                                arrayNode->elements[0])) {
                                        return boolNode->value ? "true" : "false";
                                    }
                                }
                            } else {
                                if (auto stringNode =
                                        std::dynamic_pointer_cast<StringNode>(
                                            keyValueNode->value)) {
                                    return stringNode->value;
                                } else if (auto intNode =
                                            std::dynamic_pointer_cast<IntegerNode>(
                                                keyValueNode->value)) {
                                    return std::to_string(intNode->value);
                                } else if (auto floatNode =
                                            std::dynamic_pointer_cast<FloatNode>(
                                                keyValueNode->value)) {
                                    return floatNode->value;
                                } else if (auto boolNode =
                                            std::dynamic_pointer_cast<BoolNode>(
                                                keyValueNode->value)) {
                                    return boolNode->value ? "true" : "false";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return "ERROR: Could not find value for key \"" + key + "\" in table \"" + tableName + "\" in file " + file_path;
}



