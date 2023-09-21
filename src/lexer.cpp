#include "lexer.hpp"

using namespace GTOML;

void Lexer::lex() {
    bool insideBrackets = false;
    bool insideComment = false;
    bool insideString = false;
    std::string currentTokenValue;

    auto pushToken = [&]() {
        if (!currentTokenValue.empty()) {
            Token tokenType = classify_token({ Token::IDENTIFIER, currentTokenValue, Token::EoF, Token::EoF });
            tokens.push_back({ tokenType, currentTokenValue, Token::EoF, Token::EoF });
            currentTokenValue.clear();
        }
    };

    for (auto it = content.begin(); it != content.end(); ++it) {
        char line = *it;

        switch (line) {
            case '[':
                if (!insideComment) {
                    if (insideBrackets) {
                        pushToken();
                    }
                    tokens.push_back({ Token::LEFT_BRACKET, "[", Token::EoF, Token::EoF });
                    insideBrackets = true;
                }
                break;
            case ']':
                if (!insideComment) {
                    pushToken();
                    tokens.push_back({ Token::RIGHT_BRACKET, "]", Token::EoF, Token::EoF });
                    insideBrackets = false;
                }
                break;
            case '=':
                if (!insideComment) {
                    pushToken();
                    tokens.push_back({ Token::EQUAL, "=", Token::EoF, Token::EoF });
                }
                break;
            case ',':
                if (!insideComment) {
                    pushToken();
                    tokens.push_back({ Token::COMMA, ",", Token::EoF, Token::EoF });

                }
            case '\n':
            case '\t':
            case '\r':
            case ' ':
                if (!insideComment) {
                    pushToken();
                }
                break;
            case '"':
                if (!insideComment && insideBrackets) {
                    if (insideString) {
                        pushToken();
                    }
                    insideString = !insideString;
                }
                currentTokenValue += line;
                break;
            case '#':
                insideComment = true;
                break;
            case '\0':
                pushToken();
                tokens.push_back({ Token::EoF, "\0", Token::EoF, Token::EoF });
                break;
            default:
                if (!insideComment) {
                    currentTokenValue += line;
                }
                break;
        }

        if (line == '\n' && insideComment) {
            insideComment = false;
        }
    }

    pushToken();  // Push the last token
}


Token Lexer::classify_token(const SToken& token) {
    const std::string& value = token.value;
    if (value == "[") {
        return Token::LEFT_BRACKET;
    } else if (value == "]") {
        return Token::RIGHT_BRACKET;
    } else if (value == ".") {
        return Token::DOT;
    } else if (value == "=") {
        return Token::EQUAL;
    } else if (value == "true" || value == "false") {
        return Token::BOOL;
    } else if (is_number(value)) {
        bool isFloat = (value.find('.') != std::string::npos);
        if (value[0] == '+' || value[0] == '-') {
            if (value.size() > 1 && (isdigit(value[1]) || (isFloat && isdigit(value[1])))) {
                return isFloat ? Token::FLOAT : Token::NUMBER;
            } else {
                return Token::IDENTIFIER;
            }
        } else {
            return Token::IDENTIFIER;
        }
    } else if (is_string(value)) {
        return Token::STRING;
    } else if (value == "\0") {
        return Token::EoF;
    } else {
        if (std::all_of(value.begin(), value.end(), [](char c) { return isalnum(c) || c == '_'; })) {
            return Token::IDENTIFIER;
        }
        return Token::IDENTIFIER;
    }
}




bool Lexer::is_number(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }

    size_t numUnderscores = 0;  // Count the underscores

    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '_') {
            numUnderscores++;
            continue;
        }
        if (!isdigit(str[i])) {
            return false;
        }
    }

    if (numUnderscores > 0) {
        for (size_t i = start; i < str.length(); ++i) {
            if (str[i] == '_') {
                if (i == start || i == str.length() - 1 || str[i - 1] == '_') {
                    return false;
                }
            }
        }
    }

    return true;
}




bool Lexer::is_string(const std::string& str) {
  return !str.empty() && str.front() == '"' && str.back() == '"';
}

void Lexer::toknizer() {
    for (auto& token : tokens) {
        if (token.value == "[") {
            token.type = Token::LEFT_BRACKET;
        } else if (token.value == "]") {
            token.type = Token::RIGHT_BRACKET;
        } else if (token.value == ".") {
            token.type = Token::DOT;
        } else if (token.value == ",") {
            token.type = Token::COMMA;
        } else if (token.value == "=") {
            token.type = Token::EQUAL;
        } else if (std::isdigit(token.value[0]) || (token.value[0] == '-' && std::isdigit(token.value[1]))) {
            bool isFloat = false;
            bool isNumber = true;
            for (char c : token.value) {
                if (c == '.') {
                    isFloat = true;
                } else if (!std::isdigit(c)) {
                    isNumber = false;
                    break;
                }
            }
            token.type = isFloat ? Token::FLOAT : Token::NUMBER;
        } else if (token.value[0] == '"' && token.value[token.value.length() - 1] == '"') {
            token.type = Token::STRING;
        } else if (token.value[0] == '"') {
            token.type = Token::STRING;
        } else if (token.value == "false" || token.value == "true") {
            token.type = Token::BOOL;
        } else if (token.value == "\0") {
            token.type = Token::EoF;
        } else {
            token.type = Token::IDENTIFIER;
        }
    }
}

void Lexer::print_tokens_type() {
    for (const auto& token : tokens) {
        std::string tokenTypeStr;

        switch (token.type) {
            case Token::LEFT_BRACKET:
                tokenTypeStr = "LEFT_BRACKET";
                break;
            case Token::RIGHT_BRACKET:
                tokenTypeStr = "RIGHT_BRACKET";
                break;
            case Token::EQUAL:
                tokenTypeStr = "EQUAL";
                break;
            case Token::IDENTIFIER:
                tokenTypeStr = "IDENTIFIER";
                break;
            case Token::NUMBER:
                tokenTypeStr = "NUMBER";
                break;
            case Token::STRING:
                tokenTypeStr = "STRING";
                break;
            case Token::BOOL:
                tokenTypeStr = "BOOL";
                break;
            case Token::FLOAT:
                tokenTypeStr = "FLOAT";
                break;
            case Token::DOT:
                tokenTypeStr = "DOT";
                break;
            case Token::COMMA:
                tokenTypeStr = "COMMA";
                break;
            case Token::EoF:
                tokenTypeStr = "EOF";
                break;
            default:
                tokenTypeStr = "UNKNOWN";
                break;
        }

        std::cout << tokenTypeStr << std::endl;
    }
}



void Lexer::print_token_types(int indentLevel = 0) {
  int currentIndex = 0;

  while (currentIndex < tokens.size()) {
    const SToken& token = tokens[currentIndex];
    std::string tokenTypeStr;

    switch (token.type) {
      case Token::LEFT_BRACKET:
        tokenTypeStr = "LEFT_BRACKET";
        std::cout << "["
                  << " : Type: " << tokenTypeStr << std::endl;
        ++indentLevel;
        break;
      case Token::RIGHT_BRACKET:
        --indentLevel;
        tokenTypeStr = "RIGHT_BRACKET";
        std::cout << "]"
                  << " : Type: " << tokenTypeStr << std::endl;
        break;
      case Token::EQUAL:
        tokenTypeStr = "EQUAL";
        std::cout << "= : Type: " << tokenTypeStr << std::endl;
        break;
      case Token::NUMBER:
        tokenTypeStr = "NUMBER";
        print_token(token, indentLevel, tokenTypeStr);
        break;
      case Token::STRING:
        tokenTypeStr = "STRING";
        print_token(token, indentLevel, tokenTypeStr);
        break;
      case Token::BOOL:
        tokenTypeStr = "BOOL";
        print_token(token, indentLevel, tokenTypeStr);
        break;
      case Token::FLOAT:
        tokenTypeStr = "FLOAT";
        print_token(token, indentLevel, tokenTypeStr);
        break;
      case Token::DOT:
        tokenTypeStr = "DOT";
        print_token(token, indentLevel, tokenTypeStr);
        break;
      case Token::COMMA:
        tokenTypeStr = "COMMA";
        print_token(token, indentLevel, tokenTypeStr);
        break;
      case Token::EoF:
        tokenTypeStr = "EoF";
        print_token(token, indentLevel, tokenTypeStr);
        break;
      default:
        tokenTypeStr = "IDENTIFIER";
        print_token(token, indentLevel, tokenTypeStr);
        break;
    }

    currentIndex++;
  }
}

void Lexer::print_token(const SToken& token, int indentLevel,
                        const std::string& tokenTypeStr) {
  std::cout << std::string(indentLevel, ' ') << token.value
            << " : Type: " << tokenTypeStr << std::endl;
}

void Lexer::print_tokens() {
  for (auto token : tokens) {
    std::cout << token.value << std::endl;
  }
}

void Lexer::read() {
    std::fstream file(filename.data(), std::ios::in | std::ios::binary);

    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        content.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&content[0], content.size());
        file.close();

        content.push_back('\0');
    }
}


void Lexer::print_file() {
  for (auto line : content) {
    std::cout << line;
  }
  std::cout << std::endl;
}

SToken Lexer::NextToken() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex++];
    } else {
        return CreateEofToken();  // Return an EOF token if there are no more tokens
    }
}

SToken Lexer::GetCurrentToken() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    } else {
        return CreateEofToken();  // Return an EOF token if there are no more tokens
    }
}

SToken Lexer::PrevToken() {
    if (currentTokenIndex > 0 && currentTokenIndex <= tokens.size()) {
        return tokens[currentTokenIndex - 1];
    } else {
        return CreateEmptyToken();  // Return an empty token if there is no previous token
    }
}

SToken Lexer::PervPrevToken() {
    if (currentTokenIndex > 0 && currentTokenIndex <= tokens.size()) {
        return tokens[currentTokenIndex - 2];
    } else {
        return CreateEmptyToken();  // Return an empty token if there is no previous token
    }
}

bool Lexer::hasMoreTokens() {
    return currentTokenIndex < tokens.size();
}

SToken Lexer::CreateEofToken() {
    SToken eofToken;
    eofToken.type = Token::EoF;
    return eofToken;
}

SToken Lexer::CreateEmptyToken() {
    SToken emptyToken;
    return emptyToken;
}




std::string Lexer::ToString(Token token) {
  switch (token) {
    case Token::LEFT_BRACKET:
      return "[";
    case Token::RIGHT_BRACKET:
      return "]";
    case Token::EQUAL:
      return "=";
    case Token::IDENTIFIER:
      return "IDENTIFIER";
    case Token::NUMBER:
      return "NUMBER";
    case Token::STRING:
      return "STRING";
    case Token::FLOAT:
      return "FLOAT";
    case Token::BOOL:
      return "BOOL";
    case Token::DOT:
      return ".";
    case Token::COMMA:
      return ",";
    case Token::EoF:
      return "EOF";
  }
}
