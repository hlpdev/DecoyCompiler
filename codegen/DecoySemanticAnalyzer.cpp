#include "DecoySemanticAnalyzer.hpp"

#include <sstream>

void SemanticAnalyzer::analyze() {
    firstPass();
    secondPass();
}

void SemanticAnalyzer::firstPass() {
    for (const auto& node : ast) {
        if (node.instruction.value == "cv") {
            processCv(node);
        } else if (node.instruction.value == "dfp") {
            processDfp(node);
        }

        currentAddress++;
    }
}

void SemanticAnalyzer::secondPass() {
    for (const auto& node : ast) {
        try {
            if (node.instruction.value == "av") checkAv(node);
            else if (node.instruction.value == "aav") checkAav(node);
            else if (node.instruction.value == "sav") checkSav(node);
            else if (node.instruction.value == "mav") checkMav(node);
            else if (node.instruction.value == "dav") checkDav(node);
            else if (node.instruction.value == "moav") checkMoav(node);
            else if (node.instruction.value == "inc") checkInc(node);
            else if (node.instruction.value == "dec") checkDec(node);
            else if (node.instruction.value == "p") checkP(node);
            else if (node.instruction.value == "pl") checkPl(node);
            else if (node.instruction.value == "pk") checkPk(node);
            else if (node.instruction.value == "rk") checkRk(node);
            else if (node.instruction.value == "ikd") checkIkd(node);
            else if (node.instruction.value == "mvm") checkMvm(node);
            else if (node.instruction.value == "jmp") checkJmp(node);
            else if (node.instruction.value == "cejmp") checkCejmp(node);
            else if (node.instruction.value == "cgjmp") checkCgjmp(node);
            else if (node.instruction.value == "cljmp") checkCljmp(node);
            else if (node.instruction.value == "cegjmp") checkCegjmp(node);
            else if (node.instruction.value == "celjmp") checkCeljmp(node);
            else if (node.instruction.value == "dl") checkDl(node);
        } catch (const std::exception& e) {
            std::ostringstream ss;
            ss << "At instruction " << node.instruction.value;
            ss << " (line " << node.instruction.line << "): " << e.what();
            throw std::runtime_error(ss.str());
        }
    }
}

void SemanticAnalyzer::processCv(const InstructionNode& node) {
    if (node.operands.size() != 2) throw error("cv requires 2 operands");
    const auto& typeToken = node.operands[1];
    if (typeToken.type != TokenType::TYPE) throw error("second operand must be a type");

    symbols.addVariable(node.operands[0].value, stringToType(typeToken.value));
}

void SemanticAnalyzer::processDfp(const InstructionNode& node) {
    if (node.operands.size() != 1) throw error("dfp requires 1 operand");
    symbols.addLabel(node.operands[0].value, currentAddress);
}

void SemanticAnalyzer::checkAv(const InstructionNode& node) {
    validateOperandCount(node, 2);
    const auto& var = getVariable(node.operands[0]);
    const auto& value = node.operands[1];

    if (value.type == TokenType::LITERAL) {
        validateLiteral(value.value, var.type);
    } else if (value.type == TokenType::IDENTIFIER) {
        const auto& srcVar = getVariable(value);
        validateTypeMatch(var.type, srcVar.type);
    } else {
        throw error("Invalid operand type for av");
    }
}

void SemanticAnalyzer::checkJmp(const InstructionNode& node) {
    validateOperandCount(node, 1);
    auto _ = symbols.getLabelAddress(node.operands[0].value);
}

void SemanticAnalyzer::checkAav(const InstructionNode& node) {
    validateArithmeticOp(node, "AAV");
}

void SemanticAnalyzer::checkSav(const InstructionNode& node) {
    validateArithmeticOp(node, "SAV");
}

void SemanticAnalyzer::checkMav(const InstructionNode& node) {
    validateArithmeticOp(node, "MAV");
}

void SemanticAnalyzer::checkDav(const InstructionNode& node) {
    validateArithmeticOp(node, "DAV");
}

void SemanticAnalyzer::checkMoav(const InstructionNode& node) {
    validateArithmeticOp(node, "MOAV");
}

void SemanticAnalyzer::validateArithmeticOp(const InstructionNode& node, const std::string& op) {
    validateOperandCount(node, 2);
    const auto& varInfo = getVariable(node.operands[0]);
    const Token& operand = node.operands[1];

    if (operand.type == TokenType::LITERAL) {
        validateLiteral(operand.value, varInfo.type);
    } else if (operand.type == TokenType::IDENTIFIER) {
        const auto& srcVar = getVariable(operand);
        validateTypeMatch(varInfo.type, srcVar.type);
    } else {
        throw error("Invalid operand type for " + op);
    }
}

void SemanticAnalyzer::checkInc(const InstructionNode& node) {
    validateIncDec(node);
}

void SemanticAnalyzer::checkDec(const InstructionNode& node) {
    validateIncDec(node);
}

void SemanticAnalyzer::validateIncDec(const InstructionNode& node) {
    validateOperandCount(node, 1);
    getVariable(node.operands[0]);
}

void SemanticAnalyzer::checkP(const InstructionNode& node) {
    validatePrint(node);
}

void SemanticAnalyzer::checkPl(const InstructionNode& node) {
    validatePrint(node);
}

void SemanticAnalyzer::validatePrint(const InstructionNode& node) {
    for (const auto& operand : node.operands) {
        if (operand.type != TokenType::STRING && operand.type != TokenType::IDENTIFIER) {
            throw error("Print operands must be string literals or variables");
        }
        if (operand.type == TokenType::IDENTIFIER) {
            getVariable(operand);
        }
    }
}

void SemanticAnalyzer::checkPk(const InstructionNode& node) {
    validateKeyOp(node);
}

void SemanticAnalyzer::checkRk(const InstructionNode& node) {
    validateKeyOp(node);
}

void SemanticAnalyzer::validateKeyOp(const InstructionNode& node) {
    validateOperandCount(node, 1);
    const Token& operand = node.operands[0];

    if (operand.type == TokenType::LITERAL) {
        validateUI8(operand.value);
    } else if (operand.type == TokenType::IDENTIFIER) {
        const auto& var = getVariable(operand);
        validateTypeMatch(var.type, var.type);
    } else {
        throw error("Key operation requires UI8 literal or variable");
    }
}

void SemanticAnalyzer::checkIkd(const InstructionNode& node) {
    validateOperandCount(node, 2);
    getVariable(node.operands[0]);
    const auto& resVar = getVariable(node.operands[1]);
    validateTypeMatch(Type::UI8, resVar.type);
}

void SemanticAnalyzer::checkMvm(const InstructionNode& node) {
    validateOperandCount(node, 2);
    for (const auto& operand : node.operands) {
        if (operand.type == TokenType::LITERAL) {
            validateI32(operand.value);
        } else if (operand.type == TokenType::IDENTIFIER) {
            const auto& var = getVariable(operand);
            validateTypeMatch(Type::I32, var.type);
        } else {
            throw error("mvm operands must be I32 literals or variables");
        }
    }
}

void SemanticAnalyzer::checkCejmp(const InstructionNode& node) {
    validateConditionalJump(node);
}

void SemanticAnalyzer::checkCgjmp(const InstructionNode& node) {
    validateConditionalJump(node);
}

void SemanticAnalyzer::checkCljmp(const InstructionNode& node) {
    validateConditionalJump(node);
}

void SemanticAnalyzer::checkCegjmp(const InstructionNode& node) {
    validateConditionalJump(node);
}

void SemanticAnalyzer::checkCeljmp(const InstructionNode& node) {
    validateConditionalJump(node);
}

void SemanticAnalyzer::validateConditionalJump(const InstructionNode& node) {
    validateOperandCount(node, 4);
    getVariable(node.operands[0]);
    getVariable(node.operands[1]);
    auto _ = symbols.getLabelAddress(node.operands[2].value);
    _ = symbols.getLabelAddress(node.operands[3].value);
}

void SemanticAnalyzer::checkDl(const InstructionNode& node) {
    validateOperandCount(node, 1);
    const Token& operand = node.operands[0];

    if (operand.type == TokenType::LITERAL) {
        validateUI32(operand.value);
    } else if (operand.type == TokenType::IDENTIFIER) {
        const auto& var = getVariable(operand);
        validateTypeMatch(Type::UI32, var.type);
    } else {
        throw error("dl requires UI32 literal or variable");
    }
}

void SemanticAnalyzer::validateI16(const std::string& literal) {
    int value = std::stoi(literal);
    if (value < -32768 || value > 32767) throw std::out_of_range("");
}

void SemanticAnalyzer::validateUI16(const std::string& literal) {
    unsigned long value = std::stoul(literal);
    if (value > 65535) throw std::out_of_range("");
}

void SemanticAnalyzer::validateI32(const std::string& literal) {
    long value = std::stol(literal);
    if (value < -2147483648L || value > 2147483647L) throw std::out_of_range("");
}

void SemanticAnalyzer::validateUI32(const std::string& literal) {
    unsigned long value = std::stoul(literal);
    if (value > 4294967295UL) throw std::out_of_range("");
}

void SemanticAnalyzer::validateOperandCount(const InstructionNode& node, size_t expected) {
    if (node.operands.size() != expected) {
        throw error("Expected " + std::to_string(expected) + " operands");
    }
}

void SemanticAnalyzer::validateTypeMatch(Type expected, Type actual) {
    if (expected != actual) {
        throw error("Type mismatch: expected " + typeToString(expected) + ", got " + typeToString(actual));
    }
}

void SemanticAnalyzer::validateLiteral(const std::string& literal, Type type) {
    try {
        switch (type) {
            case Type::I8:  validateI8(literal); break;
            case Type::UI8: validateUI8(literal); break;
            case Type::I16: validateI16(literal); break;
            case Type::UI16: validateUI16(literal); break;
            case Type::I32: validateI32(literal); break;
            case Type::UI32: validateUI32(literal); break;
            case Type::F32: validateF32(literal); break;
            default: throw error("Invalid type for literal assignment");
        }
    } catch (const std::exception&) {
        throw error("Value " + literal + " out of range for type " + typeToString(type));
    }
}

void SemanticAnalyzer::validateI8(const std::string& literal) {
    int value = std::stoi(literal);
    if (value < -128 || value > 127) throw std::out_of_range("");
}

void SemanticAnalyzer::validateUI8(const std::string& literal) {
    unsigned long value = std::stoul(literal);
    if (value > 255) throw std::out_of_range("");
}

void SemanticAnalyzer::validateF32(const std::string& literal) {
    auto _ = std::stof(literal); // Just check parsing
}

std::runtime_error SemanticAnalyzer::error(const std::string& msg) {
    return std::runtime_error(msg);
}

const VariableInfo& SemanticAnalyzer::getVariable(const Token& token) {
    if (token.type != TokenType::IDENTIFIER) {
        throw error("Expected variable identifier");
    }
    return symbols.getVariable(token.value);
}

Type SemanticAnalyzer::stringToType(const std::string& str) {
    static const std::unordered_map<std::string, Type> typeMap = {
        {"i8", Type::I8}, {"ui8", Type::UI8},
        {"i16", Type::I16}, {"ui16", Type::UI16},
        {"i32", Type::I32}, {"ui32", Type::UI32},
        {"f32", Type::F32}
    };
    auto it = typeMap.find(str);
    if (it == typeMap.end()) throw std::runtime_error("Invalid type specifier");
    return it->second;
}

std::string SemanticAnalyzer::typeToString(Type type) {
    static const std::unordered_map<Type, std::string> reverseMap = {
        {Type::I8, "i8"}, {Type::UI8, "ui8"},
        {Type::I16, "i16"}, {Type::UI16, "ui16"},
        {Type::I32, "i32"}, {Type::UI32, "ui32"},
        {Type::F32, "f32"}
    };
    return reverseMap.at(type);
}
