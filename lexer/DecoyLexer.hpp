#pragma once

#include <vector>
#include <string>
#include <unordered_map>

enum class TokenType {
    INSTRUCTION,
    IDENTIFIER,
    LITERAL,
    TYPE,
    LABEL,
    STRING,
    COMMA,
    END_OF_LINE
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
};

class Lexer {
    public:
    explicit Lexer(const std::string& source) : source(source), pos(0), line(1) {}

    std::vector<Token> tokenize();

    private:
    const std::string source;
    size_t pos;
    size_t line;

    const std::unordered_map<std::string, TokenType> keywords = {
        // ===== INSTRUCTIONS =====
        {"cv", TokenType::INSTRUCTION},
        {"av", TokenType::INSTRUCTION},
        {"aav", TokenType::INSTRUCTION},
        {"sav", TokenType::INSTRUCTION},
        {"mav", TokenType::INSTRUCTION},
        {"dav", TokenType::INSTRUCTION},
        {"moav", TokenType::INSTRUCTION},
        {"inc", TokenType::INSTRUCTION},
        {"dec", TokenType::INSTRUCTION},
        {"p", TokenType::INSTRUCTION},
        {"pl", TokenType::INSTRUCTION},
        {"pk", TokenType::INSTRUCTION},
        {"rk", TokenType::INSTRUCTION},
        {"ikd", TokenType::INSTRUCTION},
        {"mvm", TokenType::INSTRUCTION},
        {"dfp", TokenType::INSTRUCTION},
        {"jmp", TokenType::INSTRUCTION},
        {"cejmp", TokenType::INSTRUCTION},
        {"cgjmp", TokenType::INSTRUCTION},
        {"cljmp", TokenType::INSTRUCTION},
        {"cegjmp", TokenType::INSTRUCTION},
        {"celjmp", TokenType::INSTRUCTION},
        {"dl", TokenType::INSTRUCTION},
        {"nop", TokenType::INSTRUCTION},

        // ===== TYPES =====
        {"nt", TokenType::TYPE},
        {"i8", TokenType::TYPE},
        {"ui8", TokenType::TYPE},
        {"i16", TokenType::TYPE},
        {"ui16", TokenType::TYPE},
        {"i32", TokenType::TYPE},
        {"ui32", TokenType::TYPE},
        {"f32", TokenType::TYPE},
        {"str", TokenType::TYPE}
    };

    char peek();
    void consume();

    Token readNumber();
    Token readIdentifier();
    Token readString();
};