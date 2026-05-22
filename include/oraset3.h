#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace oraset3 {

// Token类型
enum class TokenType {
    // 关键字
    VAR, LET, CONST, FUNC, STRUCT, INTERFACE, UNION, ENUM,
    IF, ELSE, FOR, WHILE, RETURN, BREAK, CONTINUE,
    IMPORT, EXPORT, TRUE, FALSE, NIL,
    
    // 标识符和字面量
    IDENTIFIER, INTEGER, FLOAT, STRING,
    
    // 运算符
    PLUS, MINUS, MUL, DIV, MOD,
    EQ, NEQ, LT, GT, LE, GE,
    AND, OR, NOT,
    ASSIGN, COLON_ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN,
    SHL, SHR,
    BIT_AND, BIT_OR, BIT_XOR, BIT_NOT, // 位操作符
    POINTER, // * 用于指针声明和解引用
    ADDRESS, // & 用于取地址
    
    // 标点符号
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    COMMA, SEMICOLON, COLON, DOUBLE_COLON, DOT,
    
    // 特殊
    END_OF_FILE, IMPORT_DIRECTIVE
};

// Token结构
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

// AST节点类型
enum class NodeType {
    PROGRAM,
    VAR_DECL,
    FUNC_DECL,
    STRUCT_DECL,
    INTERFACE_DECL,
    UNION_DECL,    // 联合体声明
    ENUM_DECL,     // 枚举声明
    IF_STMT,
    FOR_STMT,
    WHILE_STMT,
    RETURN_STMT,
    EXPR_STMT,
    ASSIGN_EXPR,
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    INDEX_EXPR,
    MEMBER_EXPR,
    LITERAL_EXPR,
    IDENTIFIER_EXPR,
    STRUCT_INIT,
    CAST_EXPR      // 类型转换表达式
};

// AST节点基类
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual NodeType getType() const = 0;
    virtual void accept(class ASTVisitor& visitor) = 0;
};

// AST访问者模式
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(class ProgramNode& node) = 0;
    virtual void visit(class VarDeclNode& node) = 0;
    virtual void visit(class FuncDeclNode& node) = 0;
    virtual void visit(class StructDeclNode& node) = 0;
    virtual void visit(class InterfaceDeclNode& node) = 0;
    virtual void visit(class UnionDeclNode& node) = 0;
    virtual void visit(class EnumDeclNode& node) = 0;
    virtual void visit(class IfStmtNode& node) = 0;
    virtual void visit(class ForStmtNode& node) = 0;
    virtual void visit(class WhileStmtNode& node) = 0;
    virtual void visit(class ReturnStmtNode& node) = 0;
    virtual void visit(class ExprStmtNode& node) = 0;
    virtual void visit(class AssignExprNode& node) = 0;
    virtual void visit(class BinaryExprNode& node) = 0;
    virtual void visit(class UnaryExprNode& node) = 0;
    virtual void visit(class CallExprNode& node) = 0;
    virtual void visit(class IndexExprNode& node) = 0;
    virtual void visit(class MemberExprNode& node) = 0;
    virtual void visit(class LiteralExprNode& node) = 0;
    virtual void visit(class IdentifierExprNode& node) = 0;
    virtual void visit(class StructInitNode& node) = 0;
    virtual void visit(class CastExprNode& node) = 0;
};

// 具体AST节点类型定义
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations;
    
    NodeType getType() const override { return NodeType::PROGRAM; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class VarDeclNode : public ASTNode {
public:
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> initializer;
    
    NodeType getType() const override { return NodeType::VAR_DECL; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FuncDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;
    std::string returnType;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    NodeType getType() const override { return NodeType::FUNC_DECL; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class StructDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields;
    
    NodeType getType() const override { return NodeType::STRUCT_DECL; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class InterfaceDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> methods;
    
    NodeType getType() const override { return NodeType::INTERFACE_DECL; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IfStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBranch;
    std::vector<std::unique_ptr<ASTNode>> elseBranch;
    
    NodeType getType() const override { return NodeType::IF_STMT; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ForStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> update;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    NodeType getType() const override { return NodeType::FOR_STMT; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class WhileStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    NodeType getType() const override { return NodeType::WHILE_STMT; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ReturnStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expr;
    
    NodeType getType() const override { return NodeType::RETURN_STMT; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ExprStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expr;
    
    NodeType getType() const override { return NodeType::EXPR_STMT; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class AssignExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op;
    
    NodeType getType() const override { return NodeType::ASSIGN_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BinaryExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op;
    
    NodeType getType() const override { return NodeType::BINARY_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class UnaryExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> operand;
    std::string op;
    
    NodeType getType() const override { return NodeType::UNARY_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CallExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> args;
    
    NodeType getType() const override { return NodeType::CALL_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IndexExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> base;
    std::unique_ptr<ASTNode> index;
    
    NodeType getType() const override { return NodeType::INDEX_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class MemberExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> object;
    std::string property;
    std::string op;
    
    NodeType getType() const override { return NodeType::MEMBER_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class StructInitNode : public ASTNode {
public:
    std::string typeName;
    std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> fields;
    
    NodeType getType() const override { return NodeType::STRUCT_INIT; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class LiteralExprNode : public ASTNode {
public:
    std::string value;
    std::string type;
    
    NodeType getType() const override { return NodeType::LITERAL_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IdentifierExprNode : public ASTNode {
public:
    std::string name;
    
    NodeType getType() const override { return NodeType::IDENTIFIER_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// 联合体声明节点
class UnionDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields;
    
    NodeType getType() const override { return NodeType::UNION_DECL; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// 枚举声明节点
class EnumDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> values; // name -> value
    
    NodeType getType() const override { return NodeType::ENUM_DECL; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// 类型转换表达式节点
class CastExprNode : public ASTNode {
public:
    std::string targetType;
    std::unique_ptr<ASTNode> expr;
    
    NodeType getType() const override { return NodeType::CAST_EXPR; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// 词法分析器
class Lexer {
public:
    Lexer(const std::string& source);
    std::unique_ptr<Token> nextToken();
    
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    char currentChar();
    void advance();
    void skipWhitespace();
    std::string readIdentifier();
    std::string readNumber();
    std::string readString(char quote);
};

// 语法分析器
class Parser {
public:
    Parser(std::vector<std::unique_ptr<Token>> tokens);
    std::unique_ptr<ProgramNode> parse();
    
private:
    std::vector<std::unique_ptr<Token>> tokens;
    size_t pos;
    
    Token* currentToken();
    Token* peekToken();
    void advance();
    bool match(TokenType type);
    bool consume(TokenType type);
    
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<ASTNode> parseVarDecl();
    std::unique_ptr<ASTNode> parseFuncDecl();
    std::unique_ptr<ASTNode> parseStructDecl();
    std::unique_ptr<ASTNode> parseInterfaceDecl();
    std::unique_ptr<ASTNode> parseUnionDecl();
    std::unique_ptr<ASTNode> parseEnumDecl();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseIfStmt();
    std::unique_ptr<ASTNode> parseForStmt();
    std::unique_ptr<ASTNode> parseWhileStmt();
    std::unique_ptr<ASTNode> parseReturnStmt();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseShift();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseMultiplicative();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePostfix();
    std::unique_ptr<ASTNode> parsePrimary();
};

// 语义分析器
class SemanticAnalyzer : public ASTVisitor {
public:
    SemanticAnalyzer(bool sysui = false) : sysuiImported(sysui) {}
    void analyze(ProgramNode& program);
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    
private:
    std::vector<std::string> errors;
    std::map<std::string, std::string> symbolTable;
    bool sysuiImported;
    
    void visit(ProgramNode& node) override;
    void visit(VarDeclNode& node) override;
    void visit(FuncDeclNode& node) override;
    void visit(StructDeclNode& node) override;
    void visit(InterfaceDeclNode& node) override;
    void visit(UnionDeclNode& node) override;
    void visit(EnumDeclNode& node) override;
    void visit(CastExprNode& node) override;
    void visit(IfStmtNode& node) override;
    void visit(ForStmtNode& node) override;
    void visit(WhileStmtNode& node) override;
    void visit(ReturnStmtNode& node) override;
    void visit(ExprStmtNode& node) override;
    void visit(AssignExprNode& node) override;
    void visit(BinaryExprNode& node) override;
    void visit(UnaryExprNode& node) override;
    void visit(CallExprNode& node) override;
    void visit(IndexExprNode& node) override;
    void visit(MemberExprNode& node) override;
    void visit(LiteralExprNode& node) override;
    void visit(IdentifierExprNode& node) override;
    void visit(StructInitNode& node) override;
};

// 代码生成器
class CodeGenerator : public ASTVisitor {
public:
    CodeGenerator(bool sysui = false) : sysuiImported(sysui) {}
    std::string generate(ProgramNode& program);
    
private:
    std::string code;
    int indent;
    bool sysuiImported;
    
    void emit(const std::string& str);
    void emitLine(const std::string& str);
    void indentUp();
    void indentDown();
    
    void visit(ProgramNode& node) override;
    void visit(VarDeclNode& node) override;
    void visit(FuncDeclNode& node) override;
    void visit(StructDeclNode& node) override;
    void visit(InterfaceDeclNode& node) override;
    void visit(UnionDeclNode& node) override;
    void visit(EnumDeclNode& node) override;
    void visit(CastExprNode& node) override;
    void visit(IfStmtNode& node) override;
    void visit(ForStmtNode& node) override;
    void visit(WhileStmtNode& node) override;
    void visit(ReturnStmtNode& node) override;
    void visit(ExprStmtNode& node) override;
    void visit(AssignExprNode& node) override;
    void visit(BinaryExprNode& node) override;
    void visit(UnaryExprNode& node) override;
    void visit(CallExprNode& node) override;
    void visit(IndexExprNode& node) override;
    void visit(MemberExprNode& node) override;
    void visit(LiteralExprNode& node) override;
    void visit(IdentifierExprNode& node) override;
    void visit(StructInitNode& node) override;
};

// 编译器类
class Compiler {
public:
    std::string compile(const std::string& source);
};

} // namespace oraset3
