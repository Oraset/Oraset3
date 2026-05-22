#include "oraset3.h"
#include <sstream>

namespace oraset3 {

void SemanticAnalyzer::analyze(ProgramNode& program) {
    // 添加基本类型
    symbolTable["int"] = "type";
    symbolTable["float"] = "type";
    symbolTable["string"] = "type";
    symbolTable["bool"] = "type";
    symbolTable["void"] = "type";
    
    // 添加预定义标识符
    symbolTable["std"] = "namespace";
    symbolTable["cout"] = "ostream";
    symbolTable["endl"] = "stream";

    // 添加内置函数（由编译器提供）
    symbolTable["print"] = "builtin_func";
    symbolTable["println"] = "builtin_func";
    
    // 系统操作函数
    symbolTable["sys"] = "builtin_func";
    
    // 文件操作函数
    symbolTable["file"] = "namespace";
    symbolTable["file.read"] = "builtin_func";
    symbolTable["file.write"] = "builtin_func";
    symbolTable["file.exists"] = "builtin_func";
    
    // 屏幕操作函数
    symbolTable["screen"] = "namespace";
    symbolTable["screen.clear"] = "builtin_func";
    symbolTable["screen.size"] = "builtin_func";
    
    // 绘图函数
    symbolTable["draw"] = "namespace";
    symbolTable["draw.text"] = "builtin_func";
    symbolTable["draw.rect"] = "builtin_func";
    symbolTable["draw.line"] = "builtin_func";
    
    // 如果导入了 sysui，添加UI相关函数
    if (this->sysuiImported) {
        symbolTable["ui"] = "namespace";
        symbolTable["ui.msgbox"] = "builtin_func";
        symbolTable["ui.inputbox"] = "builtin_func";
        symbolTable["ui.alert"] = "builtin_func";
        symbolTable["ui.confirm"] = "builtin_func";
        symbolTable["ui.prompt"] = "builtin_func";
        symbolTable["ui.openfile"] = "builtin_func";
        symbolTable["ui.savefile"] = "builtin_func";
        symbolTable["ui.dir"] = "builtin_func";
        symbolTable["ui.color"] = "builtin_func";
    }

    visit(program);
}

void SemanticAnalyzer::visit(ProgramNode& node) {
    // 第一遍：收集所有声明
    for (auto& decl : node.declarations) {
        // 先添加函数声明到符号表
        if (decl->getType() == NodeType::FUNC_DECL) {
            FuncDeclNode& funcDecl = static_cast<FuncDeclNode&>(*decl);
            if (symbolTable.find(funcDecl.name) != symbolTable.end()) {
                std::stringstream ss;
                ss << "Function '" << funcDecl.name << "' already declared";
                errors.push_back(ss.str());
            }
            symbolTable[funcDecl.name] = "func";
        }
        // 添加变量和其他声明
        else if (decl->getType() == NodeType::VAR_DECL) {
            VarDeclNode& varDecl = static_cast<VarDeclNode&>(*decl);
            if (symbolTable.find(varDecl.name) != symbolTable.end()) {
                std::stringstream ss;
                ss << "Variable '" << varDecl.name << "' already declared";
                errors.push_back(ss.str());
            }
            symbolTable[varDecl.name] = varDecl.type;
        }
        // 添加结构体声明
        else if (decl->getType() == NodeType::STRUCT_DECL) {
            StructDeclNode& structDecl = static_cast<StructDeclNode&>(*decl);
            if (symbolTable.find(structDecl.name) != symbolTable.end()) {
                std::stringstream ss;
                ss << "Struct '" << structDecl.name << "' already declared";
                errors.push_back(ss.str());
            }
            symbolTable[structDecl.name] = "struct";
        }
        // 添加接口声明
        else if (decl->getType() == NodeType::INTERFACE_DECL) {
            InterfaceDeclNode& interfaceDecl = static_cast<InterfaceDeclNode&>(*decl);
            if (symbolTable.find(interfaceDecl.name) != symbolTable.end()) {
                std::stringstream ss;
                ss << "Interface '" << interfaceDecl.name << "' already declared";
                errors.push_back(ss.str());
            }
            symbolTable[interfaceDecl.name] = "interface";
        }
        // 添加联合体声明
        else if (decl->getType() == NodeType::UNION_DECL) {
            UnionDeclNode& unionDecl = static_cast<UnionDeclNode&>(*decl);
            if (symbolTable.find(unionDecl.name) != symbolTable.end()) {
                std::stringstream ss;
                ss << "Union '" << unionDecl.name << "' already declared";
                errors.push_back(ss.str());
            }
            symbolTable[unionDecl.name] = "union";
        }
        // 添加枚举声明
        else if (decl->getType() == NodeType::ENUM_DECL) {
            EnumDeclNode& enumDecl = static_cast<EnumDeclNode&>(*decl);
            if (symbolTable.find(enumDecl.name) != symbolTable.end()) {
                std::stringstream ss;
                ss << "Enum '" << enumDecl.name << "' already declared";
                errors.push_back(ss.str());
            }
            symbolTable[enumDecl.name] = "enum";
            // 添加枚举常量到符号表
            for (auto& value : enumDecl.values) {
                if (symbolTable.find(value.first) != symbolTable.end()) {
                    std::stringstream ss;
                    ss << "Enum value '" << value.first << "' already declared";
                    errors.push_back(ss.str());
                }
                symbolTable[value.first] = enumDecl.name;
            }
        }
    }

    // 第二遍：处理函数体
    for (auto& decl : node.declarations) {
        if (decl->getType() == NodeType::FUNC_DECL) {
            FuncDeclNode& funcDecl = static_cast<FuncDeclNode&>(*decl);
            
            // 保存当前符号表状态
            std::map<std::string, std::string> savedSymbolTable = symbolTable;
            
            // 添加参数到符号表（只检查函数内部重复）
            for (auto& param : funcDecl.params) {
                symbolTable[param.first] = param.second;
            }
            
            // 处理函数体：先收集变量声明，再处理
            for (auto& stmt : funcDecl.body) {
                if (stmt->getType() == NodeType::VAR_DECL) {
                    VarDeclNode& varDecl = static_cast<VarDeclNode&>(*stmt);
                    auto it = symbolTable.find(varDecl.name);
                    // 检查变量名是否已声明，但忽略类型名
                    if (it != symbolTable.end() && it->second != "type") {
                        std::stringstream ss;
                        ss << "Variable '" << varDecl.name << "' already declared in function '" << funcDecl.name << "'";
                        errors.push_back(ss.str());
                    }
                    symbolTable[varDecl.name] = varDecl.type;
                }
            }
            
            // 处理函数体
            for (auto& stmt : funcDecl.body) {
                stmt->accept(*this);
            }
            
            // 恢复符号表状态
            symbolTable = savedSymbolTable;
        }
        else if (decl->getType() == NodeType::VAR_DECL) {
            VarDeclNode& varDecl = static_cast<VarDeclNode&>(*decl);
            if (varDecl.initializer) {
                varDecl.initializer->accept(*this);
            }
        }
        else {
            decl->accept(*this);
        }
    }
}

void SemanticAnalyzer::visit(VarDeclNode& node) {
    // 处理初始化表达式（如果存在）
    if (node.initializer) {
        node.initializer->accept(*this);
    }
}

void SemanticAnalyzer::visit(FuncDeclNode& node) {
    // 函数声明的处理已经在 ProgramNode 的 visit 方法中完成
}

void SemanticAnalyzer::visit(StructDeclNode& node) {
    // 结构体声明的处理已经在 ProgramNode 的 visit 方法中完成
}

void SemanticAnalyzer::visit(InterfaceDeclNode& node) {
    // 接口声明的处理已经在 ProgramNode 的 visit 方法中完成
}

void SemanticAnalyzer::visit(UnionDeclNode& node) {
    // 联合体声明的处理已经在 ProgramNode 的 visit 方法中完成
}

void SemanticAnalyzer::visit(EnumDeclNode& node) {
    // 枚举声明的处理已经在 ProgramNode 的 visit 方法中完成
}

void SemanticAnalyzer::visit(CastExprNode& node) {
    // 类型转换表达式的处理
    if (node.expr) {
        node.expr->accept(*this);
    }
}

void SemanticAnalyzer::visit(IfStmtNode& node) {
    node.condition->accept(*this);
    
    for (auto& stmt : node.thenBranch) {
        stmt->accept(*this);
    }
    
    for (auto& stmt : node.elseBranch) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(ForStmtNode& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.update) node.update->accept(*this);
    
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(WhileStmtNode& node) {
    node.condition->accept(*this);
    
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(ReturnStmtNode& node) {
    if (node.expr) {
        node.expr->accept(*this);
    }
}

void SemanticAnalyzer::visit(ExprStmtNode& node) {
    node.expr->accept(*this);
}

void SemanticAnalyzer::visit(AssignExprNode& node) {
    node.left->accept(*this);
    node.right->accept(*this);
}

void SemanticAnalyzer::visit(BinaryExprNode& node) {
    node.left->accept(*this);
    node.right->accept(*this);
}

void SemanticAnalyzer::visit(UnaryExprNode& node) {
    node.operand->accept(*this);
}

void SemanticAnalyzer::visit(CallExprNode& node) {
    node.callee->accept(*this);
    
    for (auto& arg : node.args) {
        arg->accept(*this);
    }
}

void SemanticAnalyzer::visit(IndexExprNode& node) {
    node.base->accept(*this);
    node.index->accept(*this);
}

void SemanticAnalyzer::visit(MemberExprNode& node) {
    node.object->accept(*this);
}

void SemanticAnalyzer::visit(StructInitNode& node) {
    // 检查结构体是否已声明
    if (symbolTable.find(node.typeName) == symbolTable.end()) {
        std::stringstream ss;
        ss << "Undefined struct type '" << node.typeName << "'";
        errors.push_back(ss.str());
    }
    
    // 递归处理字段初始化表达式
    for (auto& field : node.fields) {
        field.second->accept(*this);
    }
}

void SemanticAnalyzer::visit(LiteralExprNode& node) {
    // 字面量无需检查
}

void SemanticAnalyzer::visit(IdentifierExprNode& node) {
    if (symbolTable.find(node.name) == symbolTable.end()) {
        std::stringstream ss;
        ss << "Undefined identifier '" << node.name << "'";
        errors.push_back(ss.str());
    }
}

} // namespace oraset3
