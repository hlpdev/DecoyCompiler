#include "DecoyParser.hpp"
std::vector<InstructionNode> Parser::parse() {
    std::vector<InstructionNode> program;
    
    while (!isAtEnd()) {
        program.push_back(parseInstruction());
    }
    
    return program;
}

bool Parser::isAtEnd() const {
    return pos >= tokens.size();
}

const Token& Parser::peek() const {
    return tokens[pos];
}

const Token& Parser::advance() {
    return tokens[pos++];
}

void Parser::consume(TokenType expected, const std::string& error) {
    if (isAtEnd() || peek().type != expected) {
        throw parseError(error);
    }

    advance();
}

std::runtime_error Parser::parseError(const std::string& message) {
    size_t line = isAtEnd() ? tokens.back().line : peek().line;
    return std::runtime_error("Line " + std::to_string(line) + ": " + message);
}

InstructionNode Parser::parseInstruction() {
    InstructionNode node;
    node.instruction = advance();

    const std::string& inst = node.instruction.value;

    if (inst == "cv") {
        parseCv(node);
    } else if (inst == "av") {
        parseAv(node);
    } else if (inst == "aav" || inst == "sav" || inst == "mav" || inst == "dav" || inst == "moav") {
        parseMathAssignment(node);
    } else if (inst == "inc" || inst == "dec") {
        parseIncDec(node);
    } else if (inst == "p" || inst == "pl") {
        parsePrint(node);
    } else if (inst == "pk" || inst == "rk") {
        parseKeyOperation(node);
    } else if (inst == "ikd") {
        parseIkd(node);
    } else if (inst == "mvm") {
        parseMvm(node);
    } else if (inst == "dfp") {
        parseDfp(node);
    } else if (inst == "jmp") {
        parseJmp(node);
    } else if (inst == "cejmp" || inst == "cgjmp" || inst == "cljmp" || inst == "cegjmp" || inst == "celjmp") {
        parseConditionalJmp(node);
    } else if (inst == "dl") {
        parseDl(node);
    } else if (inst == "nop") {
        parseNop(node);
    } else {
        throw parseError("Unknown instruction " + inst);
    }

    consume(TokenType::END_OF_LINE, "Expected end of line after instruction");
    return node;
}

void Parser::parseCv(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected a variable name"));
    node.operands.push_back(consumeType("Expected a variable type (e.g., ui8, i32, etc"));
}

void Parser::parseAv(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected a variable name"));
    consumeValueOperand(node);
}

void Parser::parseMathAssignment(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected a variable name"));
    consumeValueOperand(node);
}

void Parser::parseIncDec(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected a variable name"));
}

void Parser::parsePrint(InstructionNode& node) {
    while (peek().type == TokenType::STRING || peek().type == TokenType::IDENTIFIER) {
        node.operands.push_back(advance());
    }

    if (node.operands.empty()) {
        throw parseError("Print instruction requires at least one operand");
    }
}

void Parser::parseKeyOperation(InstructionNode& node) {
    if (peek().type == TokenType::LITERAL || peek().type == TokenType::IDENTIFIER) {
        node.operands.push_back(advance());
    } else {
        throw parseError("Key operation requires literal or variable");
    }
}

void Parser::parseIkd(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected a variable name"));
    node.operands.push_back(consumeIdentifier("Expected result variable"));
}

void Parser::parseMvm(InstructionNode& node) {
    consumeValueOperand(node);
    consumeValueOperand(node);
}

void Parser::parseDfp(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected a label name"));
}

void Parser::parseJmp(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected a label name"));
}

void Parser::parseConditionalJmp(InstructionNode& node) {
    node.operands.push_back(consumeIdentifier("Expected first operand variable"));
    node.operands.push_back(consumeIdentifier("Expected second operand variable"));
    node.operands.push_back(consumeIdentifier("Expected true label"));
    node.operands.push_back(consumeIdentifier("Expected false label"));
}

void Parser::parseDl(InstructionNode& node) {
    consumeValueOperand(node);
}

void Parser::parseNop(InstructionNode& node) {
    if (peek().type != TokenType::END_OF_LINE) {
        throw parseError("NOP instruction takes no operands");
    }
}

Token Parser::consumeIdentifier(const std::string& error) {
    consume(TokenType::IDENTIFIER, error);
    return tokens[pos - 1];
}

Token Parser::consumeType(const std::string& error) {
    consume(TokenType::TYPE, error);
    return tokens[pos - 1];
}

void Parser::consumeValueOperand(InstructionNode& node) {
    if (peek().type == TokenType::LITERAL || peek().type == TokenType::IDENTIFIER) {
        node.operands.push_back(advance());
    } else {
        throw parseError("Expected literal value or variable");
    }
}
