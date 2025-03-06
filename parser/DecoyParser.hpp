#pragma once
#include "../lexer/DecoyLexer.hpp"

#include <stdexcept>

enum class Instruction : uint8_t {
    CV = 0, // Create Variable (ex: cv var ui8)
    AV = 1, // Assign Variable (ex: av var 6)
    AAV = 2, // Assign Variable via Addition (ex: aav var 4)
    SAV = 3, // Assign Variable via Subtraction (ex: sav var 2)
    MAV = 4, // Assign Variable via Multiplication (ex: mav var 2)
    DAV = 5, // Assign Variable via Division (ex: dav var 4)
    MOAV = 6, // Assign Variable via Modulus (ex: moav var 2)
    INC = 7, // Increments a Variable by 1 (Essentially just "aav var 1") (ex: inc var)
    DEC = 8, // Decrements a Variable by 1 (Essentially just "sav var 1") (ex: dec var)
    P = 9, // Print Consecutive Variables (ex: p var) (ex: p var var2)
    PL = 10, // Print Consecutive Variables then continue to next line (ex: pl var) (ex: pl var var2)
    PK = 11, // Press the Given KeyCode (ex: pk 1) (ex: pk var)
    RK = 12, // Release the Given KeyCode (ex: rk 1) (ex: rk var)
    IKD = 13, // Checks if the Given KeyCode is being pressed (1 if yes, 0 if no) (ex: ikd key resvar)
    MVM = 14, // Move the Mouse the given X & Y (i32, i32) (ex: mvm 5 -5)
    DFP = 15, // Define a jump position (ex: dfp tag)
    JMP = 16, // Jump to the given position (ex: jmp tag)
    CEJMP = 17, // Conditional Equality Jump (Jump to 1st provided jump position if condition is true, else jump to 2nd) (ex: cejmp var var2 tag tag2)
    CGJMP = 18, // Conditional Greater Than Jump (Jump to 1st provided jump position if condition is true, else jump to 2nd) (ex: cgjmp var var2 tag tag2)
    CLJMP = 19, // Conditional Less Than Jump (Jump to 1st provided jump position if condition is true, else jump to 2nd) (ex: cljmp var var2 tag tag2)
    CEGJMP = 20, // Conditional Greater Than or Equal To Jump (Jump to 1st provided jump position if condition is true, else jump to 2nd) (ex: cegjmp var var2 tag tag2)
    CELJMP = 21, // Conditional Less Than or Equal To Jump (Jump to 1st provided jump position if condition is true, else jump to 2nd) (ex: celjmp var var2 tag tag2)
    DL = 22, // Delay the program by the given milliseconds (ex: dl 5000) (ex: dl var)
    NOP = 255, // No Operation (Do nothing) (ex: nop)
};

enum class Type : uint8_t {
    NT = 0, // No Type
    I8 = 1, // Signed 8-bit Integer
    UI8 = 2, // Unsigned 8-bit Integer
    I16 = 3, // Signed 16-bit Integer
    UI16 = 4, // Unsigned 16-bit Integer
    I32 = 5, // Signed 32-bit Integer
    UI32 = 6, // Unsigned 32-bit Integer
    F32 = 7, // 32-bit Float
    STR = 8, // String
};

struct InstructionNode {
    Token instruction;
    std::vector<Token> operands;
};

class Parser {
    public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

    std::vector<InstructionNode> parse();

    private:
    const std::vector<Token>& tokens;
    size_t pos;

    bool isAtEnd() const;
    const Token& peek() const;
    const Token& advance();

    void consume(TokenType expected, const std::string& error);

    std::runtime_error parseError(const std::string& message);

    InstructionNode parseInstruction();

    void parseCv(InstructionNode& node);
    void parseAv(InstructionNode& node);
    void parseMathAssignment(InstructionNode& node);
    void parseIncDec(InstructionNode& node);
    void parsePrint(InstructionNode& node);
    void parseKeyOperation(InstructionNode& node);
    void parseIkd(InstructionNode& node);
    void parseMvm(InstructionNode& node);
    void parseDfp(InstructionNode& node);
    void parseJmp(InstructionNode& node);
    void parseConditionalJmp(InstructionNode& node);
    void parseDl(InstructionNode& node);
    void parseNop(InstructionNode& node);

    Token consumeIdentifier(const std::string& error);
    Token consumeType(const std::string& error);
    
    void consumeValueOperand(InstructionNode& node);
};