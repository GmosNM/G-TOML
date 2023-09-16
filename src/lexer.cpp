#include "lexer.hpp"

void Lexer::lex() {
  bool insideBrackets = false;
  bool insideString = false;
  bool insideComment = false;
  std::string currentTokenValue;

  auto pushToken = [&]() {
    if (!currentTokenValue.empty()) {
      Token tokenType = classify_token(
          {Token::IDENTIFIER, currentTokenValue, Token::EoF, Token::EoF});
      tokens.push_back({tokenType, currentTokenValue, Token::EoF, Token::EoF});
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
          tokens.push_back({Token::LEFT_BRACKET, "[", Token::EoF, Token::EoF});
          insideBrackets = true;
        }
        break;
      case ']':
        if (!insideComment) {
          pushToken();
          tokens.push_back({Token::RIGHT_BRACKET, "]", Token::EoF, Token::EoF});
          insideBrackets = false;
        }
        break;
      case '=':
        if (!insideComment) {
          pushToken();
          tokens.push_back({Token::EQUAL, "=", Token::EoF, Token::EoF});
        }
        break;
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

  pushToken();
}

Token Lexer::classify_token(const SToken& token) {
  const std::string& value = token.value;

  if (value == "[") {
    return Token::LEFT_BRACKET;
  } else if (value == "]") {
    return Token::RIGHT_BRACKET;
  } else if (value == "=") {
    return Token::EQUAL;
  } else if (value == "true" || value == "false") {
    return Token::BOOL;
  } else if (is_number(value)) {
    return Token::NUMBER;
  } else if (is_string(value)) {
    return Token::STRING;
  } else {
    return Token::IDENTIFIER;
  }
}

bool Lexer::is_number(const std::string& str) {
  for (char c : str) {
    if (!isdigit(c)) {
      return false;
    }
  }
  return !str.empty();
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
    } else if (token.value == "=") {
      token.type = Token::EQUAL;
    } else if (std::isdigit(token.value[0])) {
      token.type = Token::NUMBER;
    } else if (token.value[0] == '"' &&
               token.value[token.value.length() - 1] == '"') {
      token.type = Token::STRING;
    } else if (token.value[0] == '"') {
      token.type = Token::STRING;
    } else if (token.value == "false" || token.value == "true") {
      token.type = Token::BOOL;

    } else {
      token.type = Token::IDENTIFIER;
    }
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
    SToken token;
    token.type = tokenTypes[currentTokenIndex];
    token.value = tokens[currentTokenIndex].value;

    if (currentTokenIndex + 1 < tokens.size()) {
      token.next = tokenTypes[currentTokenIndex + 1];
    } else {
      token.next = Token::EoF;
    }

    if (currentTokenIndex > 0) {
      token.prev = tokenTypes[currentTokenIndex - 1];
    } else {
      token.prev = Token::EoF;
    }

    currentTokenIndex++;
    return token;
  } else {
    SToken token;
    token.type = Token::EoF;
    token.value = "";
    token.next = Token::EoF;
    token.prev = Token::EoF;
    return token;
  }
}

SToken Lexer::GetCurrentToken() {
  if (currentTokenIndex < tokens.size()) {
    SToken token;
    token.type = tokenTypes[currentTokenIndex];
    token.value = tokens[currentTokenIndex].value;

    if (currentTokenIndex + 1 < tokens.size()) {
      token.next = tokenTypes[currentTokenIndex + 1];
    } else {
      token.next = Token::EoF;
    }

    if (currentTokenIndex > 0) {
      token.prev = tokenTypes[currentTokenIndex - 1];
    } else {
      token.prev = Token::EoF;
    }

    return token;
  } else {
    SToken token;
    token.type = Token::EoF;
    token.value = "";
    token.next = Token::EoF;
    token.prev = Token::EoF;
    return token;
  }
}

SToken Lexer::PervToken() {
  if (currentTokenIndex > 0) {
    SToken token;
    token.type = tokenTypes[currentTokenIndex - 1];
    token.value = tokens[currentTokenIndex - 1].value;

    if (currentTokenIndex < tokens.size()) {
      token.next = tokenTypes[currentTokenIndex];
    } else {
      token.next = Token::EoF;
    }

    if (currentTokenIndex > 1) {
      token.prev = tokenTypes[currentTokenIndex - 2];
    } else {
      token.prev = Token::EoF;
    }

    currentTokenIndex--;
    return token;
  } else {
    SToken token;
    token.type = Token::EoF;
    token.value = "";
    token.next = Token::EoF;
    token.prev = Token::EoF;
    return token;
  }
}
