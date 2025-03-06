#pragma once

#include <vector>

#include "DecoySymbolTable.hpp"

class CodeGenerator {
    public:
    CodeGenerator(const SymbolTable& symbols)
        : symbols(symbols) {}

    std::vector<uint8_t> generate(const std::vector<InstructionNode>& ast);

    private:
    const SymbolTable& symbols;
    std::vector<uint8_t> bytecode;
    std::unordered_map<std::string, size_t> labelAddresses;

    void buildLabelMap(const std::vector<InstructionNode>& ast);

    void generateInstruction(const InstructionNode& node);

    void emitOperand(const Token& operand);
    void emitLiteral(const std::string& value, Type type);
    void emitVariable(const Token& varToken);
    void emitLabel(const Token& labelToken);

    Type inferLiteralType(const std::string& literal);

    void emitByte(uint8_t value);
    void emitUI32(uint32_t value);
    void emitI32(int32_t value);
    void emitUI16(uint16_t value);
    void emitI16(int16_t value);
    void emitI8(int8_t value);
    void emitUI8(uint8_t value);
    void emitF32(float value);
    void emitString(const std::string& str);
    void emitType(Type type);

    size_t calculateInstructionSize(const InstructionNode& node);

    Instruction instructionToOpcode(const std::string& inst);

    size_t operandSize(const Token& operand);
};

inline size_t CodeGenerator::operandSize(const Token& operand) {
    if (operand.type == TokenType::LITERAL) {
        Type type = inferLiteralType(operand.value);
        switch (type) {
            case Type::I8: case Type::UI8: return 1 + 1; // type + value
            case Type::I16: case Type::UI16: return 1 + 2;
            case Type::I32: case Type::UI32: case Type::F32: return 1 + 4;
            default: throw std::runtime_error("Invalid literal type");
        }
    }
    return 4; // Variable reference
}
