#include "oraset3.h"
#include <cctype>
#include <unordered_map>

namespace oraset3 {

Lexer::Lexer(const std::string& source) 
    : source(source), pos(0), line(1), column(1) {}

char Lexer::currentChar() {
    if (pos >= source.size()) return '\0';
    return source[pos];
}

void Lexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
}

void Lexer::skipWhitespace() {
    while (isspace(currentChar())) {
        advance();
    }
}

std::string Lexer::readIdentifier() {
    std::string result;
    while (isalnum(currentChar()) || currentChar() == '_') {
        result += currentChar();
        advance();
    }
    return result;
}

std::string Lexer::readNumber() {
    std::string result;
    bool hasDecimal = false;
    
    while (isdigit(currentChar()) || (currentChar() == '.' && !hasDecimal)) {
        if (currentChar() == '.') hasDecimal = true;
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string Lexer::readString(char quote) {
    std::string result;
    advance(); // 跳过引号
    
    while (currentChar() != quote && currentChar() != '\0') {
        if (currentChar() == '\\') {
            advance();
            switch (currentChar()) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += currentChar(); break;
            }
        } else {
            result += currentChar();
        }
        advance();
    }
    
    advance(); // 跳过结束引号
    return result;
}

std::unique_ptr<Token> Lexer::nextToken() {
    skipWhitespace();
    
    char c = currentChar();
    int tokenLine = line;
    int tokenColumn = column;
    
    if (c == '\0') {
        return std::make_unique<Token>(TokenType::END_OF_FILE, "", tokenLine, tokenColumn);
    }
    
    if (isalpha(c) || c == '_') {
        std::string id = readIdentifier();
        
        static const std::unordered_map<std::string, TokenType> keywords = {
            {"var", TokenType::VAR},
            {"let", TokenType::LET},
            {"const", TokenType::CONST},
            {"func", TokenType::FUNC},
            {"struct", TokenType::STRUCT},
            {"interface", TokenType::INTERFACE},
            {"union", TokenType::UNION},
            {"enum", TokenType::ENUM},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"for", TokenType::FOR},
            {"while", TokenType::WHILE},
            {"return", TokenType::RETURN},
            {"break", TokenType::BREAK},
            {"continue", TokenType::CONTINUE},
            {"import", TokenType::IMPORT},
            {"export", TokenType::EXPORT},
            {"true", TokenType::TRUE},
            {"false", TokenType::FALSE},
            {"nil", TokenType::NIL}
        };
        
        auto it = keywords.find(id);
        if (it != keywords.end()) {
            return std::make_unique<Token>(it->second, id, tokenLine, tokenColumn);
        }
        return std::make_unique<Token>(TokenType::IDENTIFIER, id, tokenLine, tokenColumn);
    }
    
    if (isdigit(c)) {
        std::string num = readNumber();
        if (num.find('.') != std::string::npos) {
            return std::make_unique<Token>(TokenType::FLOAT, num, tokenLine, tokenColumn);
        }
        return std::make_unique<Token>(TokenType::INTEGER, num, tokenLine, tokenColumn);
    }
    
    if (c == '"' || c == '\'') {
        std::string str = readString(c);
        return std::make_unique<Token>(TokenType::STRING, str, tokenLine, tokenColumn);
    }
    
    advance();
    
    switch (c) {
        case '+': {
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::ADD_ASSIGN, "+=", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::PLUS, "+", tokenLine, tokenColumn);
        }
        case '-': {
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::SUB_ASSIGN, "-=", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::MINUS, "-", tokenLine, tokenColumn);
        }
        case '/': {
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::DIV_ASSIGN, "/=", tokenLine, tokenColumn);
            }
            // 跳过注释
            if (currentChar() == '/') {
                while (currentChar() != '\n' && currentChar() != '\0') {
                    advance();
                }
                return nextToken();
            }
            return std::make_unique<Token>(TokenType::DIV, "/", tokenLine, tokenColumn);
        }
        case '%': return std::make_unique<Token>(TokenType::MOD, "%", tokenLine, tokenColumn);
        case '=': {
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::EQ, "==", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::ASSIGN, "=", tokenLine, tokenColumn);
        }
        case '!': {
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::NEQ, "!=", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::NOT, "!", tokenLine, tokenColumn);
        }
        case '<': {
            if (currentChar() == '<') {
                advance();
                return std::make_unique<Token>(TokenType::SHL, "<<", tokenLine, tokenColumn);
            }
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::LE, "<=", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::LT, "<", tokenLine, tokenColumn);
        }
        case '>': {
            if (currentChar() == '>') {
                advance();
                return std::make_unique<Token>(TokenType::SHR, ">>", tokenLine, tokenColumn);
            }
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::GE, ">=", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::GT, ">", tokenLine, tokenColumn);
        }
        case '&': {
            if (currentChar() == '&') {
                advance();
                return std::make_unique<Token>(TokenType::AND, "&&", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::BIT_AND, "&", tokenLine, tokenColumn);
        }
        case '|': {
            if (currentChar() == '|') {
                advance();
                return std::make_unique<Token>(TokenType::OR, "||", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::BIT_OR, "|", tokenLine, tokenColumn);
        }
        case '^': return std::make_unique<Token>(TokenType::BIT_XOR, "^", tokenLine, tokenColumn);
        case '~': return std::make_unique<Token>(TokenType::BIT_NOT, "~", tokenLine, tokenColumn);
        case '*': {
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::MUL_ASSIGN, "*=", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::MUL, "*", tokenLine, tokenColumn);
        }
        case '(': return std::make_unique<Token>(TokenType::LPAREN, "(", tokenLine, tokenColumn);
        case ')': return std::make_unique<Token>(TokenType::RPAREN, ")", tokenLine, tokenColumn);
        case '{': return std::make_unique<Token>(TokenType::LBRACE, "{", tokenLine, tokenColumn);
        case '}': return std::make_unique<Token>(TokenType::RBRACE, "}", tokenLine, tokenColumn);
        case '[': return std::make_unique<Token>(TokenType::LBRACKET, "[", tokenLine, tokenColumn);
        case ']': return std::make_unique<Token>(TokenType::RBRACKET, "]", tokenLine, tokenColumn);
        case ',': return std::make_unique<Token>(TokenType::COMMA, ",", tokenLine, tokenColumn);
        case ';': return std::make_unique<Token>(TokenType::SEMICOLON, ";", tokenLine, tokenColumn);
        case ':': {
            if (currentChar() == ':') {
                advance();
                return std::make_unique<Token>(TokenType::DOUBLE_COLON, "::", tokenLine, tokenColumn);
            }
            if (currentChar() == '=') {
                advance();
                return std::make_unique<Token>(TokenType::COLON_ASSIGN, ":=", tokenLine, tokenColumn);
            }
            return std::make_unique<Token>(TokenType::COLON, ":", tokenLine, tokenColumn);
        }
        case '.': return std::make_unique<Token>(TokenType::DOT, ".", tokenLine, tokenColumn);
        case '$': {
            advance(); // 跳过 $
            std::string id = readIdentifier();
            return std::make_unique<Token>(TokenType::IMPORT_DIRECTIVE, id, tokenLine, tokenColumn);
        }
    }
    
    return std::make_unique<Token>(TokenType::END_OF_FILE, "", tokenLine, tokenColumn);
}

} // namespace oraset3