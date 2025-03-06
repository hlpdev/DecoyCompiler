#include "DecoyLexer.hpp"

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < source.length()) {
        char current = peek();

        if (std::isdigit(current) || current == '-') {
            tokens.push_back(readNumber());
        } else if (std::isalpha(current) || current == '_') {
            tokens.push_back(readIdentifier());
        } else if (current == '"') {
            tokens.push_back(readString());
        } else if (current == ',') {
            consume();
            tokens.push_back({ TokenType::COMMA, ",", line });
        } else if (current == '\n') {
            consume();
            tokens.push_back({ TokenType::END_OF_LINE, "EOL", line });
            line++;
        } else {
            consume();
        }
    }

    if (!tokens.empty() && tokens.back().type != TokenType::END_OF_LINE) {
        tokens.push_back({ TokenType::END_OF_LINE, "EOL", line });
    }
    
    return tokens;
}

char Lexer::peek() {
    return (pos < source.length()) ? source[pos] : '\0';
}

void Lexer::consume() {
    pos++;
}

Token Lexer::readNumber() {
    size_t start = pos;
    bool isFloat = false;
    
    if (peek() == '-') consume();
    
    while (true) {
        char c = peek();
        if (std::isdigit(c)) {
            consume();
        } else if (c == '.' && !isFloat) {
            isFloat = true;
            consume();
        } else {
            break;
        }
    }
    
    return {TokenType::LITERAL, source.substr(start, pos - start), line};
}

Token Lexer::readIdentifier() {
    size_t start = pos;
    while (std::isalnum(peek())) consume();
    std::string id = source.substr(start, pos - start);

    if (keywords.contains(id)) {
        return { keywords.at(id), id, line };
    }

    return { TokenType::IDENTIFIER, id, line };
}

Token Lexer::readString() {
    consume();
    size_t start = pos;

    while (peek() != '"' && peek() != '\0') consume();
    std::string str = source.substr(start, pos - start);
    if (peek() == '"') consume();
    return { TokenType::STRING, str, line };
}
