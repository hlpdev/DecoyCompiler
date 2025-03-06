#include "DecoySymbolTable.hpp"

void SymbolTable::addVariable(const std::string& name, Type type) {
    if (variables.count(name)) {
        throw std::runtime_error("Redeclaration of variable '" + name + "'");
    }
        
    size_t size = getTypeSize(type);
    variables[name] = {
        .type = type,
        .size = size,
        .offset = currentOffset
    };
    currentOffset += size;
}

const VariableInfo& SymbolTable::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it == variables.end()) {
        throw std::runtime_error("Undefined variable '" + name + "'");
    }
    return it->second;
}

void SymbolTable::addLabel(const std::string& name, size_t address) {
    if (labels.count(name)) {
        throw std::runtime_error("Redeclaration of label '" + name + "'");
    }
    labels[name] = {address};
}

size_t SymbolTable::getLabelAddress(const std::string& name) const {
    auto it = labels.find(name);
    if (it == labels.end()) {
        throw std::runtime_error("Undefined label '" + name + "'");
    }
    return it->second.instruction_address;
}

bool SymbolTable::isVariable(const std::string& name) const {
    return variables.contains(name);
}

size_t SymbolTable::getTypeSize(Type type) {
    static const std::unordered_map<Type, size_t> typeSizes = {
        {Type::NT,   0},
        {Type::I8,   1},
        {Type::UI8,  1},
        {Type::I16,  2},
        {Type::UI16, 2},
        {Type::I32,  4},
        {Type::UI32, 4},
        {Type::F32,  4},
        {Type::STR,  0}  // Strings aren't stored in variables
    };
        
    auto it = typeSizes.find(type);
    if (it == typeSizes.end()) {
        throw std::runtime_error("Unknown type size");
    }
    return it->second;
}
