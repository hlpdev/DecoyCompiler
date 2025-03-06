#include <iostream>
#include <fstream>
#include <filesystem>
#include <miniz/miniz.h>

#include "lexer/DecoyLexer.hpp"
#include "parser/DecoyParser.hpp"
#include "codegen/DecoySymbolTable.hpp"
#include "codegen/DecoySemanticAnalyzer.hpp"
#include "codegen/DecoyCodeGenerator.hpp"

#include <iomanip>

// Helper to convert TokenType to a string
std::string getTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::INSTRUCTION: return "INSTRUCTION";
        case TokenType::IDENTIFIER:  return "IDENTIFIER";
        case TokenType::LITERAL:     return "LITERAL";
        case TokenType::TYPE:        return "TYPE";
        case TokenType::LABEL:       return "LABEL";
        case TokenType::STRING:      return "STRING";
        case TokenType::COMMA:       return "COMMA";
        case TokenType::END_OF_LINE: return "END_OF_LINE";
        default:                     return "UNKNOWN";
    }
}

void printTokens(const std::vector<Token>& tokens, std::string filename) {
    std::cout << "\nToken Stream (" + filename + "):\n";
    std::cout << "==============\n";
    for (const auto& token : tokens) {
        std::cout << "Line " << token.line << ": " 
                  << std::setw(12) << std::left << getTokenTypeName(token.type)
                  << " '" << token.value << "'\n";
    }
    std::cout << "==============\n\n";
}


void printAST(const std::vector<InstructionNode>& ast, std::string filename) {
    std::cout << "Parsed Program (" + filename + "):\n";
    std::cout << "----------------\n";
    
    // First pass to find maximum line number width
    size_t maxLineWidth = 0;
    for (const auto& node : ast) {
        std::string lineStr = std::to_string(node.instruction.line);
        maxLineWidth = std::max(maxLineWidth, lineStr.size());
    }

    for (const auto& node : ast) {
        // Print instruction line number with dynamic alignment
        std::cout << "Line " 
                  << std::setw(maxLineWidth) << std::right << node.instruction.line 
                  << ": ";

        // Print instruction mnemonic with fixed width
        std::cout << std::setw(6) << std::left << node.instruction.value << " ";

        // Print operands
        for (const auto& operand : node.operands) {
            std::cout << "[" << getTokenTypeName(operand.type) << ": \""
                      << operand.value << "\"] ";
        }
        std::cout << "\n";
    }
    std::cout << "----------------\n";
}

struct CompilationUnit {
    std::string source_path;
    std::vector<uint8_t> bytecode;
};

int main(int argc, char* argv[]) {
    std::vector<std::string> inputFiles;
    std::string outputFile;

    bool showHelp = false, debugLexer = false, debugParser = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            while (++i < argc && argv[i][0] != '-') {
                inputFiles.emplace_back(argv[i]);
            }
            --i;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-h") {
            showHelp = true;
        } else if (arg == "--debug-lexer") {
            debugLexer = true;
        } else if (arg == "--debug-parser") {
            debugParser = true;
        }
    }
    
    if (showHelp || inputFiles.empty() || outputFile.empty()) {
        std::cerr << "Usage: " << argv[0] << " [--debug-lexer] [--debug-parser] -i script1.dc script2.dc -o output.xex\n";
        return 1;
    }

    std::vector<CompilationUnit> units;
    for (const auto& input : inputFiles) {
        try {
            CompilationUnit unit;
            unit.source_path = input;

            std::ifstream sourceFile(input);
            if (!sourceFile.is_open()) {
                throw std::runtime_error("Could not open source file: " + input);
            }
            std::string source((std::istreambuf_iterator(sourceFile)),
                         std::istreambuf_iterator<char>());
            
            Lexer lexer(source);
            auto tokens = lexer.tokenize();

            if (debugLexer) {
                printTokens(tokens, input);
            }
            
            Parser parser(tokens);
            auto ast = parser.parse();

            if (debugParser) {
                printAST(ast, input);
            }
            
            SymbolTable symbols;
            SemanticAnalyzer analyzer(symbols, ast);
            analyzer.analyze();
            
            CodeGenerator generator(symbols);
            unit.bytecode = generator.generate(ast);

            if (unit.bytecode.empty()) {
                throw std::runtime_error("Generated bytecode is empty");
            }
            
            units.push_back(unit);
        } catch (const std::exception& e) {
            std::cerr << "\nCompilation Failed!\nError: " << e.what() << '\n';
            return 1;
        }
    }

    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(mz_zip_archive));

    if (!mz_zip_writer_init_file(&zipArchive, outputFile.c_str(), 0)) {
        std::cerr << "Failed to create output binary\n";
        return 1;
    }

    for (const auto& unit : units) {
        std::string entryName = std::filesystem::path(unit.source_path).stem().string() + ".xexm";

        if (unit.bytecode.empty()) {
            std::cerr << "Empty bytecode for " << entryName << '\n';
            continue;
        }
        
        if (!mz_zip_writer_add_mem(&zipArchive, entryName.c_str(), unit.bytecode.data(), unit.bytecode.size(), MZ_DEFAULT_COMPRESSION)) {
            std::cerr << "Failed to add " << entryName << " to output binary\n";
            mz_zip_writer_end(&zipArchive);
            return 1;
        }
    }

    if (!mz_zip_writer_finalize_archive(&zipArchive) || !mz_zip_writer_end(&zipArchive)) {
        std::cerr << "Failed to finalize output binary\n";
        return 1;
    }

    std::cout << "Successfully compiled " << units.size() << " scripts to " << outputFile << '\n';

    return 0;
}
