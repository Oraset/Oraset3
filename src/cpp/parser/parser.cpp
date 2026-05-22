#include "oraset3.h"
#include <stdexcept>

namespace oraset3 {

Parser::Parser(std::vector<std::unique_ptr<Token>> tokens) 
    : tokens(std::move(tokens)), pos(0) {}

Token* Parser::currentToken() {
    if (pos >= tokens.size()) return nullptr;
    return tokens[pos].get();
}

Token* Parser::peekToken() {
    if (pos + 1 >= tokens.size()) return nullptr;
    return tokens[pos + 1].get();
}

void Parser::advance() {
    if (pos < tokens.size()) pos++;
}

bool Parser::match(TokenType type) {
    Token* t = currentToken();
    return t && t->type == type;
}

bool Parser::consume(TokenType type) {
    if (match(type)) {
        advance();
        return true;
    }
    return false;
}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    
    while (currentToken()) {
        program->declarations.push_back(parseDeclaration());
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    if (match(TokenType::VAR) || match(TokenType::LET) || match(TokenType::CONST)) {
        return parseVarDecl();
    }
    if (match(TokenType::FUNC)) {
        return parseFuncDecl();
    }
    if (match(TokenType::STRUCT)) {
        return parseStructDecl();
    }
    if (match(TokenType::INTERFACE)) {
        return parseInterfaceDecl();
    }
    if (match(TokenType::UNION)) {
        return parseUnionDecl();
    }
    if (match(TokenType::ENUM)) {
        return parseEnumDecl();
    }
    // 处理导入指令 $xxx
    if (match(TokenType::IMPORT_DIRECTIVE)) {
        // 跳过导入指令，因为编译器已经在 lexer 阶段处理了
        // 返回一个空的表达式语句作为占位符
        return std::make_unique<ExprStmtNode>();
    }
    // 检查是否是 := 声明（x := 10） - 不消费标识符，让 parseVarDecl 处理
    if (match(TokenType::IDENTIFIER) && peekToken() && peekToken()->type == TokenType::COLON_ASSIGN) {
        return parseVarDecl();
    }
    // 检查是否是 类型 变量名 = 值 的声明
    if (match(TokenType::IDENTIFIER) && peekToken() && peekToken()->type == TokenType::IDENTIFIER) {
        Token* nextNext = nullptr;
        if (pos + 2 < tokens.size()) {
            nextNext = tokens[pos + 2].get();
        }
        if (nextNext && (nextNext->type == TokenType::ASSIGN || nextNext->type == TokenType::COLON_ASSIGN)) {
            // 这是一个带类型的变量声明，让 parseVarDecl 处理
            return parseVarDecl();
        }
    }
    
    // 检查控制流语句
    if (match(TokenType::IF)) {
        return parseIfStmt();
    }
    if (match(TokenType::WHILE)) {
        return parseWhileStmt();
    }
    if (match(TokenType::FOR)) {
        return parseForStmt();
    }
    if (match(TokenType::RETURN)) {
        return parseReturnStmt();
    }
    if (match(TokenType::BREAK)) {
        auto stmt = std::make_unique<ExprStmtNode>();
        advance();
        consume(TokenType::SEMICOLON);
        return stmt;
    }
    if (match(TokenType::CONTINUE)) {
        auto stmt = std::make_unique<ExprStmtNode>();
        advance();
        consume(TokenType::SEMICOLON);
        return stmt;
    }
    
    // 顶层表达式语句（无需函数包装）
    std::unique_ptr<ASTNode> expr = parseExpression();
    consume(TokenType::SEMICOLON);
    
    // 将表达式包装在 ExprStmtNode 中
    auto stmt = std::make_unique<ExprStmtNode>();
    stmt->expr = std::move(expr);
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parseVarDecl() {
    auto decl = std::make_unique<VarDeclNode>();
    bool hasKeyword = match(TokenType::VAR) || match(TokenType::LET) || match(TokenType::CONST);
    
    if (hasKeyword) {
        advance(); // 跳过 var/let/const 关键字
    }
    
    // 获取第一个标识符
    if (!consume(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected variable name");
    }
    std::string firstToken = tokens[pos - 1]->value;
    
    // 检查是否是 类型 变量名 = 值 的语法
    // 如果下一个是标识符，且下下个是赋值操作符
    bool hasTypeAnnotation = false;
    if (!hasKeyword && peekToken() && peekToken()->type == TokenType::IDENTIFIER) {
        Token* nextNext = nullptr;
        if (pos + 2 < tokens.size()) {
            nextNext = tokens[pos + 2].get();
        }
        if (nextNext && (nextNext->type == TokenType::ASSIGN || nextNext->type == TokenType::COLON_ASSIGN)) {
            hasTypeAnnotation = true;
        }
    }
    
    if (hasTypeAnnotation) {
        // 第一个标识符是类型
        decl->type = firstToken;
        // 获取变量名
        if (!consume(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected variable name");
        }
        decl->name = tokens[pos - 1]->value;
    } else {
        // 第一个标识符是变量名
        decl->name = firstToken;
        // 检查类型注解 (var x int = 10)
        if (consume(TokenType::IDENTIFIER) && !match(TokenType::ASSIGN) && !match(TokenType::COLON_ASSIGN)) {
            decl->type = tokens[pos - 1]->value;
        }
    }
    
    // 处理赋值
    if (consume(TokenType::ASSIGN) || consume(TokenType::COLON_ASSIGN)) {
        decl->initializer = parseExpression();
    }
    
    consume(TokenType::SEMICOLON);
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseFuncDecl() {
    auto decl = std::make_unique<FuncDeclNode>();
    advance(); // 跳过 func
    
    // 函数名
    if (!consume(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected function name");
    }
    decl->name = tokens[pos - 1]->value;
    
    // 参数列表
    consume(TokenType::LPAREN);
    while (match(TokenType::IDENTIFIER)) {
        std::string paramName = tokens[pos]->value;
        advance();
        std::string paramType = "auto";
        if (match(TokenType::COLON)) {
            advance();
            if (consume(TokenType::IDENTIFIER)) {
                paramType = tokens[pos - 1]->value;
            }
        }
        decl->params.push_back({paramName, paramType});
        
        if (!match(TokenType::RPAREN)) {
            consume(TokenType::COMMA);
        }
    }
    consume(TokenType::RPAREN);
    
    // 返回类型（可选）
    if (match(TokenType::COLON)) {
        advance();
        if (consume(TokenType::IDENTIFIER)) {
            decl->returnType = tokens[pos - 1]->value;
        }
    }
    
    // 函数体
    consume(TokenType::LBRACE);
    while (!match(TokenType::RBRACE) && currentToken()) {
        decl->body.push_back(parseStatement());
    }
    consume(TokenType::RBRACE);
    
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseStructDecl() {
    auto decl = std::make_unique<StructDeclNode>();
    advance(); // 跳过 struct
    
    if (!consume(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected struct name");
    }
    decl->name = tokens[pos - 1]->value;
    
    consume(TokenType::LBRACE);
    while (!match(TokenType::RBRACE) && currentToken()) {
        if (!consume(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected field name");
        }
        std::string fieldName = tokens[pos - 1]->value;
        
        std::string fieldType = "auto";
        if (consume(TokenType::COLON)) {
            if (consume(TokenType::IDENTIFIER)) {
                fieldType = tokens[pos - 1]->value;
            }
        }
        
        decl->fields.push_back({fieldName, fieldType});
        consume(TokenType::SEMICOLON);
    }
    consume(TokenType::RBRACE);
    
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseInterfaceDecl() {
    auto decl = std::make_unique<InterfaceDeclNode>();
    advance(); // 跳过 interface
    
    if (!consume(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected interface name");
    }
    decl->name = tokens[pos - 1]->value;
    
    consume(TokenType::LBRACE);
    while (!match(TokenType::RBRACE) && currentToken()) {
        if (!consume(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected method name");
        }
        std::string methodName = tokens[pos - 1]->value;
        
        consume(TokenType::LPAREN);
        consume(TokenType::RPAREN);
        
        std::string returnType = "void";
        if (match(TokenType::COLON)) {
            advance();
            if (consume(TokenType::IDENTIFIER)) {
                returnType = tokens[pos - 1]->value;
            }
        }
        
        decl->methods.push_back({methodName, returnType});
        consume(TokenType::SEMICOLON);
    }
    consume(TokenType::RBRACE);
    
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseUnionDecl() {
    auto decl = std::make_unique<UnionDeclNode>();
    advance(); // 跳过 union
    
    if (!consume(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected union name");
    }
    decl->name = tokens[pos - 1]->value;
    
    consume(TokenType::LBRACE);
    while (!match(TokenType::RBRACE) && currentToken()) {
        if (!consume(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected field name");
        }
        std::string fieldName = tokens[pos - 1]->value;
        
        std::string fieldType = "auto";
        if (consume(TokenType::COLON)) {
            if (consume(TokenType::IDENTIFIER)) {
                fieldType = tokens[pos - 1]->value;
            }
        }
        
        decl->fields.push_back({fieldName, fieldType});
        consume(TokenType::SEMICOLON);
    }
    consume(TokenType::RBRACE);
    consume(TokenType::SEMICOLON);  // 添加分号消费
    
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseEnumDecl() {
    auto decl = std::make_unique<EnumDeclNode>();
    advance(); // 跳过 enum
    
    if (!consume(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected enum name");
    }
    decl->name = tokens[pos - 1]->value;
    
    consume(TokenType::LBRACE);
    while (!match(TokenType::RBRACE) && currentToken()) {
        if (!consume(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected enum value name");
        }
        std::string valueName = tokens[pos - 1]->value;
        std::string value = "";
        
        if (consume(TokenType::ASSIGN)) {
            if (consume(TokenType::INTEGER)) {
                value = tokens[pos - 1]->value;
            }
        }
        
        decl->values.push_back({valueName, value});
        
        if (!match(TokenType::RBRACE)) {
            consume(TokenType::COMMA);
        }
    }
    consume(TokenType::RBRACE);
    consume(TokenType::SEMICOLON);  // 添加分号消费
    
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::VAR) || match(TokenType::LET) || match(TokenType::CONST)) {
        return parseVarDecl();
    }
    if (match(TokenType::IF)) {
        return parseIfStmt();
    }
    if (match(TokenType::WHILE)) {
        return parseWhileStmt();
    }
    if (match(TokenType::FOR)) {
        return parseForStmt();
    }
    if (match(TokenType::RETURN)) {
        return parseReturnStmt();
    }
    if (match(TokenType::BREAK)) {
        auto stmt = std::make_unique<ExprStmtNode>();
        advance();
        consume(TokenType::SEMICOLON);
        return stmt;
    }
    if (match(TokenType::CONTINUE)) {
        auto stmt = std::make_unique<ExprStmtNode>();
        advance();
        consume(TokenType::SEMICOLON);
        return stmt;
    }
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    
    // 检查是否是 := 声明（x := 10）
    if (match(TokenType::IDENTIFIER) && peekToken() && peekToken()->type == TokenType::COLON_ASSIGN) {
        return parseVarDecl();
    }
    
    // 可能是表达式语句
    std::unique_ptr<ASTNode> expr = parseExpression();
    consume(TokenType::SEMICOLON);
    
    // 将表达式包装在 ExprStmtNode 中
    auto stmt = std::make_unique<ExprStmtNode>();
    stmt->expr = std::move(expr);
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    auto block = std::make_unique<ExprStmtNode>();
    advance(); // 跳过 {
    
    // 简单处理：只处理第一个语句
    if (!match(TokenType::RBRACE) && currentToken()) {
        block->expr = parseStatement();
    }
    
    // 跳过其余语句直到找到 }
    while (!match(TokenType::RBRACE) && currentToken()) {
        advance();
    }
    
    consume(TokenType::RBRACE);
    return block;
}

std::unique_ptr<ASTNode> Parser::parseIfStmt() {
    auto stmt = std::make_unique<IfStmtNode>();
    advance(); // 跳过 if
    
    consume(TokenType::LPAREN);
    stmt->condition = parseExpression();
    consume(TokenType::RPAREN);
    
    // then branch
    if (match(TokenType::LBRACE)) {
        stmt->thenBranch.push_back(parseBlock());
    } else {
        stmt->thenBranch.push_back(parseStatement());
    }
    
    // else branch
    if (match(TokenType::ELSE)) {
        advance();
        if (match(TokenType::IF)) {
            stmt->elseBranch.push_back(parseIfStmt());
        } else if (match(TokenType::LBRACE)) {
            stmt->elseBranch.push_back(parseBlock());
        } else {
            stmt->elseBranch.push_back(parseStatement());
        }
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parseWhileStmt() {
    auto stmt = std::make_unique<WhileStmtNode>();
    advance(); // 跳过 while
    
    consume(TokenType::LPAREN);
    stmt->condition = parseExpression();
    consume(TokenType::RPAREN);
    
    if (match(TokenType::LBRACE)) {
        stmt->body.push_back(parseBlock());
    } else {
        stmt->body.push_back(parseStatement());
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parseForStmt() {
    auto stmt = std::make_unique<ForStmtNode>();
    advance(); // 跳过 for
    
    consume(TokenType::LPAREN);
    
    // 初始化
    if (!match(TokenType::SEMICOLON)) {
        stmt->init = parseExpression();
    }
    consume(TokenType::SEMICOLON);
    
    // 条件
    if (!match(TokenType::SEMICOLON)) {
        stmt->condition = parseExpression();
    }
    consume(TokenType::SEMICOLON);
    
    // 更新
    if (!match(TokenType::RPAREN)) {
        stmt->update = parseExpression();
    }
    consume(TokenType::RPAREN);
    
    if (match(TokenType::LBRACE)) {
        stmt->body.push_back(parseBlock());
    } else {
        stmt->body.push_back(parseStatement());
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parseReturnStmt() {
    auto stmt = std::make_unique<ReturnStmtNode>();
    advance(); // 跳过 return
    
    if (!match(TokenType::SEMICOLON)) {
        stmt->expr = parseExpression();
    }
    
    consume(TokenType::SEMICOLON);
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    std::unique_ptr<ASTNode> left = parseLogicalOr();
    
    if (match(TokenType::ASSIGN) || match(TokenType::COLON_ASSIGN)) {
        auto assign = std::make_unique<AssignExprNode>();
        assign->left = std::move(left);
        assign->op = match(TokenType::ASSIGN) ? "=" : ":=";
        advance();
        assign->right = parseAssignment();
        return assign;
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr() {
    std::unique_ptr<ASTNode> left = parseLogicalAnd();
    
    while (match(TokenType::OR)) {
        auto binary = std::make_unique<BinaryExprNode>();
        binary->left = std::move(left);
        binary->op = "||";
        advance();
        binary->right = parseLogicalAnd();
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    std::unique_ptr<ASTNode> left = parseEquality();
    
    while (match(TokenType::AND)) {
        auto binary = std::make_unique<BinaryExprNode>();
        binary->left = std::move(left);
        binary->op = "&&";
        advance();
        binary->right = parseEquality();
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    std::unique_ptr<ASTNode> left = parseComparison();
    
    while (match(TokenType::EQ) || match(TokenType::NEQ)) {
        auto binary = std::make_unique<BinaryExprNode>();
        binary->left = std::move(left);
        binary->op = match(TokenType::EQ) ? "==" : "!=";
        advance();
        binary->right = parseComparison();
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    std::unique_ptr<ASTNode> left = parseShift();
    
    while (match(TokenType::LT) || match(TokenType::GT) || match(TokenType::LE) || match(TokenType::GE)) {
        auto binary = std::make_unique<BinaryExprNode>();
        binary->left = std::move(left);
        if (match(TokenType::LT)) binary->op = "<";
        else if (match(TokenType::GT)) binary->op = ">";
        else if (match(TokenType::LE)) binary->op = "<=";
        else binary->op = ">=";
        advance();
        binary->right = parseShift();
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseShift() {
    std::unique_ptr<ASTNode> left = parseAdditive();
    
    while (match(TokenType::SHL) || match(TokenType::SHR) || 
           match(TokenType::BIT_AND) || match(TokenType::BIT_OR) || match(TokenType::BIT_XOR)) {
        auto binary = std::make_unique<BinaryExprNode>();
        binary->left = std::move(left);
        if (match(TokenType::SHL)) binary->op = "<<";
        else if (match(TokenType::SHR)) binary->op = ">>";
        else if (match(TokenType::BIT_AND)) binary->op = "&";
        else if (match(TokenType::BIT_OR)) binary->op = "|";
        else binary->op = "^";
        advance();
        binary->right = parseAdditive();
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    std::unique_ptr<ASTNode> left = parseMultiplicative();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        auto binary = std::make_unique<BinaryExprNode>();
        binary->left = std::move(left);
        binary->op = match(TokenType::PLUS) ? "+" : "-";
        advance();
        binary->right = parseMultiplicative();
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicative() {
    std::unique_ptr<ASTNode> left = parseUnary();
    
    while (match(TokenType::MUL) || match(TokenType::DIV) || match(TokenType::MOD)) {
        auto binary = std::make_unique<BinaryExprNode>();
        binary->left = std::move(left);
        if (match(TokenType::MUL)) binary->op = "*";
        else if (match(TokenType::DIV)) binary->op = "/";
        else binary->op = "%";
        advance();
        binary->right = parseUnary();
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS) || 
        match(TokenType::MUL) || match(TokenType::BIT_AND) || match(TokenType::BIT_NOT)) {
        auto unary = std::make_unique<UnaryExprNode>();
        if (match(TokenType::NOT)) unary->op = "!";
        else if (match(TokenType::MINUS)) unary->op = "-";
        else if (match(TokenType::MUL)) unary->op = "*";
        else if (match(TokenType::BIT_AND)) unary->op = "&";
        else unary->op = "~";
        advance();
        unary->operand = parseUnary();
        return unary;
    }
    
    return parsePostfix();
}

std::unique_ptr<ASTNode> Parser::parsePostfix() {
    std::unique_ptr<ASTNode> expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::LPAREN)) {
            // 函数调用
            auto call = std::make_unique<CallExprNode>();
            call->callee = std::move(expr);
            
            consume(TokenType::LPAREN);
            if (!match(TokenType::RPAREN)) {
                do {
                    call->args.push_back(parseExpression());
                } while (consume(TokenType::COMMA));
            }
            consume(TokenType::RPAREN);
            
            expr = std::move(call);
        } else if (match(TokenType::LBRACE)) {
            // 结构体实例化，如 Point{x: 10, y: 20}
            auto structInit = std::make_unique<StructInitNode>();
            structInit->typeName = static_cast<IdentifierExprNode*>(expr.get())->name;
            
            advance(); // 跳过 {
            while (!match(TokenType::RBRACE) && currentToken()) {
                if (!consume(TokenType::IDENTIFIER)) {
                    throw std::runtime_error("Expected field name in struct initializer");
                }
                std::string fieldName = tokens[pos - 1]->value;
                
                consume(TokenType::COLON);
                std::unique_ptr<ASTNode> fieldValue = parseExpression();
                
                structInit->fields.push_back({fieldName, std::move(fieldValue)});
                
                if (!match(TokenType::RBRACE)) {
                    consume(TokenType::COMMA);
                }
            }
            consume(TokenType::RBRACE);
            
            expr = std::move(structInit);
        } else if (match(TokenType::DOT)) {
            // 成员访问
            advance();
            if (!consume(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected member name");
            }
            auto member = std::make_unique<MemberExprNode>();
            member->object = std::move(expr);
            member->property = tokens[pos - 1]->value;
            member->op = ".";
            expr = std::move(member);
        } else if (match(TokenType::LBRACKET)) {
            // 数组索引
            advance();
            auto index = std::make_unique<IndexExprNode>();
            index->base = std::move(expr);
            index->index = parseExpression();
            consume(TokenType::RBRACKET);
            expr = std::move(index);
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    // 类型转换: (type)expr
    if (match(TokenType::LPAREN) && peekToken() && peekToken()->type == TokenType::IDENTIFIER) {
        advance(); // 跳过 LPAREN
        std::string targetType = tokens[pos]->value;
        advance(); // 跳过类型名
        if (consume(TokenType::RPAREN)) {
            auto cast = std::make_unique<CastExprNode>();
            cast->targetType = targetType;
            cast->expr = parseUnary(); // 使用 parseUnary 允许括号内的表达式
            return cast;
        }
    }
    
    if (match(TokenType::INTEGER) || match(TokenType::FLOAT)) {
        auto lit = std::make_unique<LiteralExprNode>();
        lit->value = tokens[pos]->value;
        lit->type = match(TokenType::FLOAT) ? "float" : "int";
        advance();
        return lit;
    }
    
    if (match(TokenType::STRING)) {
        auto lit = std::make_unique<LiteralExprNode>();
        lit->value = tokens[pos]->value;
        lit->type = "string";
        advance();
        return lit;
    }
    
    if (match(TokenType::TRUE)) {
        auto lit = std::make_unique<LiteralExprNode>();
        lit->value = "true";
        lit->type = "bool";
        advance();
        return lit;
    }
    
    if (match(TokenType::FALSE)) {
        auto lit = std::make_unique<LiteralExprNode>();
        lit->value = "false";
        lit->type = "bool";
        advance();
        return lit;
    }
    
    if (match(TokenType::IDENTIFIER)) {
        auto ident = std::make_unique<IdentifierExprNode>();
        ident->name = tokens[pos]->value;
        advance();
        return ident;
    }
    
    if (match(TokenType::LPAREN)) {
        advance();
        std::unique_ptr<ASTNode> expr = parseExpression();
        consume(TokenType::RPAREN);
        return expr;
    }
    
    throw std::runtime_error("Unexpected token");
}

} // namespace oraset3
