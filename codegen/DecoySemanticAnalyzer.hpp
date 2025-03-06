#pragma once

#include "DecoySymbolTable.hpp"

class SemanticAnalyzer {
    public:
    SemanticAnalyzer(SymbolTable& symbols, const std::vector<InstructionNode>& ast)
        : symbols(symbols), ast(ast) {}

    void analyze();

    private:
    SymbolTable& symbols;
    const std::vector<InstructionNode>& ast;
    size_t currentAddress = 0;

    void firstPass();
    void secondPass();

    void processCv(const InstructionNode& node);
    void processDfp(const InstructionNode& node);
    
    void checkAv(const InstructionNode& node);
    void checkJmp(const InstructionNode& node);
    void checkAav(const InstructionNode& node);
    void checkSav(const InstructionNode& node);
    void checkMav(const InstructionNode& node);
    void checkDav(const InstructionNode& node);
    void checkMoav(const InstructionNode& node);

    void validateArithmeticOp(const InstructionNode& node, const std::string& op);

    void checkInc(const InstructionNode& node);
    void checkDec(const InstructionNode& node);

    void validateIncDec(const InstructionNode& node);

    void checkP(const InstructionNode& node);
    void checkPl(const InstructionNode& node);

    void validatePrint(const InstructionNode& node);

    void checkPk(const InstructionNode& node);
    void checkRk(const InstructionNode& node);

    void validateKeyOp(const InstructionNode& node);

    void checkIkd(const InstructionNode& node);

    void checkMvm(const InstructionNode& node);

    void checkCejmp(const InstructionNode& node);
    void checkCgjmp(const InstructionNode& node);
    void checkCljmp(const InstructionNode& node);
    void checkCegjmp(const InstructionNode& node);
    void checkCeljmp(const InstructionNode& node);

    void validateConditionalJump(const InstructionNode& node);

    void checkDl(const InstructionNode& node);
    
    void validateOperandCount(const InstructionNode& node, size_t expected);
    void validateTypeMatch(Type expected, Type actual);
    void validateLiteral(const std::string& literal, Type type);
    void validateI8(const std::string& literal);
    void validateUI8(const std::string& literal);
    void validateI16(const std::string& literal);
    void validateUI16(const std::string& literal);
    void validateI32(const std::string& literal);
    void validateUI32(const std::string& literal);
    void validateF32(const std::string& literal);

    std::runtime_error error(const std::string& msg);

    const VariableInfo& getVariable(const Token& token);

    static Type stringToType(const std::string& str);

    static std::string typeToString(Type type);
};