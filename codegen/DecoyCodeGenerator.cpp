#include "DecoyCodeGenerator.hpp"

std::vector<uint8_t> CodeGenerator::generate(const std::vector<InstructionNode>& ast) {
    bytecode.clear();

    buildLabelMap(ast);

    for (const auto& node : ast) {
        generateInstruction(node);
    }

    return bytecode;
}

void CodeGenerator::buildLabelMap(const std::vector<InstructionNode>& ast) {
    size_t address = 0;

    for (const auto& node : ast) {
        if (node.instruction.value == "dfp") {
            labelAddresses[node.operands[0].value] = address;
        }
        address += calculateInstructionSize(node);
    }
}

void CodeGenerator::generateInstruction(const InstructionNode& node) {
    auto opcode = instructionToOpcode(node.instruction.value);
    emitByte(static_cast<uint8_t>(opcode));

    const std::string& inst = node.instruction.value;

    if (inst == "cv") {
        // cv var type: [var_name][type]
        emitString(node.operands[0].value);
        emitType(symbols.getVariable(node.operands[0].value).type);
    }
    else if (inst == "av") {
        // av var value: [var_offset][value]
        emitVariable(node.operands[0]);
        emitOperand(node.operands[1]);
    }
    else if (inst == "aav" || inst == "sav" || inst == "mav" || 
             inst == "dav" || inst == "moav") {
        // aav var value: [var_offset][value]
        emitVariable(node.operands[0]);
        emitOperand(node.operands[1]);
    }
    else if (inst == "inc" || inst == "dec") {
        // inc var: [var_offset]
        emitVariable(node.operands[0]);
    }
    else if (inst == "p" || inst == "pl") {
        // p args...: [arg1][arg2]...
        for (const auto& operand : node.operands) {
            if (operand.type == TokenType::STRING) {
                emitString(operand.value);
            } else {
                emitVariable(operand);
            }
        }
    }
    else if (inst == "pk" || inst == "rk") {
        // pk value: [value]
        emitOperand(node.operands[0]);
    }
    else if (inst == "ikd") {
        // ikd key res: [key_offset][res_offset]
        emitVariable(node.operands[0]);
        emitVariable(node.operands[1]);
    }
    else if (inst == "mvm") {
        // mvm x y: [x][y]
        emitOperand(node.operands[0]);
        emitOperand(node.operands[1]);
    }
    else if (inst == "dfp") {
        // dfp label: (no code, handled in label map)
    }
    else if (inst == "jmp") {
        // jmp label: [address]
        emitLabel(node.operands[0]);
    }
    else if (inst == "cejmp" || inst == "cgjmp" || inst == "cljmp" ||
             inst == "cegjmp" || inst == "celjmp") {
        // cejmp a b t f: [a_offset][b_offset][t_addr][f_addr]
        emitVariable(node.operands[0]);
        emitVariable(node.operands[1]);
        emitLabel(node.operands[2]);
        emitLabel(node.operands[3]);
    }
    else if (inst == "dl") {
        // dl duration: [duration]
        emitOperand(node.operands[0]);
    }
    else if (inst == "nop") {
        // nop: no operands
    }
}

void CodeGenerator::emitOperand(const Token& operand) {
    if (operand.type == TokenType::LITERAL) {
        emitLiteral(operand.value, inferLiteralType(operand.value));
    } else if (operand.type == TokenType::IDENTIFIER) {
        if (symbols.isVariable(operand.value)) {
            emitVariable(operand);
        } else {
            emitLabel(operand);
        }
    }
}

void CodeGenerator::emitLiteral(const std::string& value, Type type) {
    emitByte(static_cast<uint8_t>(type));
    switch (type) {
        case Type::I8: emitI8(std::stoi(value)); break;
        case Type::UI8: emitUI8(std::stoul(value)); break;
        case Type::I16: emitI16(std::stoi(value)); break;
        case Type::UI16: emitUI16(std::stoul(value)); break;
        case Type::I32: emitI32(std::stol(value)); break;
        case Type::UI32: emitUI32(std::stoul(value)); break;
        case Type::F32: emitF32(std::stof(value)); break;
        default: throw std::runtime_error("Unsupported literal type");
    }
}

void CodeGenerator::emitVariable(const Token& varToken) {
    const auto& var = symbols.getVariable(varToken.value);
    emitUI32(var.offset);
}

void CodeGenerator::emitLabel(const Token& labelToken) {
    size_t address = labelAddresses.at(labelToken.value);
    emitUI32(static_cast<uint32_t>(address));
}

Type CodeGenerator::inferLiteralType(const std::string& literal) {
    if (literal.find('.') != std::string::npos) return Type::F32;
    if (literal[0] == '-') return Type::I32;
    return Type::UI32;
}

void CodeGenerator::emitByte(uint8_t value) {
    bytecode.push_back(value);
}

void CodeGenerator::emitUI32(uint32_t value) {
    for (int i = 0; i < 4; i++) {
        emitByte((value >> (8 * i)) & 0xFF);
    }
}

void CodeGenerator::emitI32(int32_t value) {
    emitUI32(value);
}

void CodeGenerator::emitUI16(uint16_t value) {
    emitByte(static_cast<uint8_t>(value & 0xFF));
    emitByte(static_cast<uint8_t>((value >> 8) & 0xFF));
}

void CodeGenerator::emitI16(int16_t value) {
    emitUI16(static_cast<uint16_t>(value));
}

void CodeGenerator::emitI8(int8_t value) {
    emitUI8(static_cast<uint8_t>(value));
}

void CodeGenerator::emitUI8(uint8_t value) {
    emitByte(value);
}

void CodeGenerator::emitF32(float value) {
    uint32_t binary;
    memcpy(&binary, &value, sizeof(float));
    emitUI32(binary);
}

void CodeGenerator::emitString(const std::string& str) {
    emitUI32(str.size());
    for (char c : str) emitByte(static_cast<uint8_t>(c));
}

void CodeGenerator::emitType(Type type) {
    emitByte(static_cast<uint8_t>(type));
}

size_t CodeGenerator::calculateInstructionSize(const InstructionNode& node) {
    const std::string& inst = node.instruction.value;
    size_t size = 1; // Opcode

    if (inst == "cv") {
        // [4-byte len][name][1-byte type]
        size += 4 + node.operands[0].value.size() + 1;
    }
    else if (inst == "av" || inst == "aav" || inst == "sav" || 
             inst == "mav" || inst == "dav" || inst == "moav") {
        // [4-byte var][operand]
        size += 4 + operandSize(node.operands[1]);
             }
    else if (inst == "inc" || inst == "dec") {
        // [4-byte var]
        size += 4;
    }
    else if (inst == "p" || inst == "pl") {
        for (const auto& operand : node.operands) {
            size += (operand.type == TokenType::STRING) ? 
                (4 + operand.value.size()) : 4;
        }
    }
    else if (inst == "pk" || inst == "rk" || inst == "dl") {
        size += operandSize(node.operands[0]);
    }
    else if (inst == "ikd") {
        size += 4 + 4; // Two variables
    }
    else if (inst == "mvm") {
        size += operandSize(node.operands[0]) + operandSize(node.operands[1]);
    }
    else if (inst == "jmp") {
        size += 4; // Address
    }
    else if (inst == "cejmp" || inst == "cgjmp" || inst == "cljmp" ||
             inst == "cegjmp" || inst == "celjmp") {
        size += 4 + 4 + 4 + 4; // Two vars, two addresses
             }
    // nop: no size addition

    return size;
}

Instruction CodeGenerator::instructionToOpcode(const std::string& inst) {
    static const std::unordered_map<std::string, Instruction> opcodeMap = {
        {"cv", Instruction::CV},
        {"av", Instruction::AV},
        {"aav", Instruction::AAV},
        {"sav", Instruction::SAV},
        {"mav", Instruction::MAV},
        {"dav", Instruction::DAV},
        {"moav", Instruction::MOAV},
        {"inc", Instruction::INC},
        {"dec", Instruction::DEC},
        {"p", Instruction::P},
        {"pl", Instruction::PL},
        {"pk", Instruction::PK},
        {"rk", Instruction::RK},
        {"ikd", Instruction::IKD},
        {"mvm", Instruction::MVM},
        {"dfp", Instruction::DFP},
        {"jmp", Instruction::JMP},
        {"cejmp", Instruction::CEJMP},
        {"cgjmp", Instruction::CGJMP},
        {"cljmp", Instruction::CLJMP},
        {"cegjmp", Instruction::CEGJMP},
        {"celjmp", Instruction::CELJMP},
        {"dl", Instruction::DL},
        {"nop", Instruction::NOP}
    };

    auto it = opcodeMap.find(inst);
    if (it == opcodeMap.end()) {
        throw std::runtime_error("Unknown instruction: " + inst);
    }
    return it->second;
}