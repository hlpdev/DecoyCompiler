#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "../parser/DecoyParser.hpp"

struct VariableInfo {
    Type type;
    size_t size;
    size_t offset;
};

struct LabelInfo {
    size_t instruction_address;
};

class SymbolTable {
    public:
    SymbolTable() = default;

    void addVariable(const std::string& name, Type);
    const VariableInfo& getVariable(const std::string& name) const;

    void addLabel(const std::string& name, size_t address);
    size_t getLabelAddress(const std::string& name) const;

    size_t getTotalMemorySize() const { return currentOffset; }
    void reset() { variables.clear(); labels.clear(); currentOffset = 0; }

    bool isVariable(const std::string& name) const;

    private:
    std::unordered_map<std::string, VariableInfo> variables;
    std::unordered_map<std::string, LabelInfo> labels;
    size_t currentOffset = 0;

    static size_t getTypeSize(Type type);
};