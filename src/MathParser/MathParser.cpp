#include <cctype>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <stack>
#include <iostream>  // For debug (optional)

enum class TokenType { NUMBER, OPERATOR, LPAREN, RPAREN, IDENTIFIER, END, INVALID };

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer(const std::string& expr) : expr(expr), pos(0) {}

    Token next() {
        while (pos < expr.length() && isspace(expr[pos])) pos++;

        if (pos >= expr.length()) return { TokenType::END, "" };

        char ch = expr[pos];

        if (isdigit(ch) || ch == '.') {
            std::string num;
            int dotCount = 0;
            while (pos < expr.length() && (isdigit(expr[pos]) || expr[pos] == '.')) {
                if (expr[pos] == '.') {
                    dotCount++;
                    if (dotCount > 1) break; // stop on second dot (invalid number)
                }
                num += expr[pos++];
            }
            return { TokenType::NUMBER, num };
        }

        if (isalpha(ch)) {
            std::string id;
            while (pos < expr.length() && (isalnum(expr[pos]) || expr[pos] == '_')) {
                id += expr[pos++];
            }
            return { TokenType::IDENTIFIER, id };
        }

        pos++;

        switch (ch) {
            case '+': case '-': case '*': case '/': case '^':
                return { TokenType::OPERATOR, std::string(1, ch) };
            case '(':
                return { TokenType::LPAREN, "(" };
            case ')':
                return { TokenType::RPAREN, ")" };
            default:
                // Instead of throwing, return INVALID token to let parser handle
                return { TokenType::INVALID, std::string(1, ch) };
        }
    }

private:
    std::string expr;
    size_t pos;
};

class Parser {
public:
    Parser(const std::string& expr) : lexer(expr) {
        advance();
    }

    double parse() {
        double result = expression();
        if (current.type != TokenType::END) {
            throw std::runtime_error("Unexpected token: " + current.value);
        }
        return result;
    }

private:
    Lexer lexer;
    Token current;

    void advance() {
        current = lexer.next();
        if (current.type == TokenType::INVALID) {
            throw std::runtime_error("Invalid character in expression: " + current.value);
        }
    }

    double expression() {
        double result = term();
        while (current.type == TokenType::OPERATOR && (current.value == "+" || current.value == "-")) {
            std::string op = current.value;
            advance();
            double right = term();
            result = (op == "+") ? result + right : result - right;
        }
        return result;
    }

    double term() {
        double result = factor();
        while (current.type == TokenType::OPERATOR && (current.value == "*" || current.value == "/")) {
            std::string op = current.value;
            advance();
            double right = factor();
            if (op == "/" && right == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            result = (op == "*") ? result * right : result / right;
        }
        return result;
    }

    double factor() {
        double result = unary();
        while (current.type == TokenType::OPERATOR && current.value == "^") {
            advance();
            double right = unary();
            result = std::pow(result, right);
        }
        return result;
    }

    double unary() {
        if (current.type == TokenType::OPERATOR && current.value == "+") {
            advance();
            return unary();
        }
        if (current.type == TokenType::OPERATOR && current.value == "-") {
            advance();
            return -unary();
        }
        return primary();
    }

    double primary() {
        if (current.type == TokenType::NUMBER) {
            double val = std::stod(current.value);
            advance();
            return val;
        }
        if (current.type == TokenType::IDENTIFIER) {
            std::string id = current.value;
            advance();

            if (id == "pi") return M_PI;
            if (id == "e") return M_E;

            if (current.type == TokenType::LPAREN) {
                advance();
                double arg = expression();
                if (current.type != TokenType::RPAREN) throw std::runtime_error("Missing closing parenthesis");
                advance();
                return callFunction(id, arg);
            }

            throw std::runtime_error("Unknown identifier: " + id);
        }
        if (current.type == TokenType::LPAREN) {
            advance();
            double val = expression();
            if (current.type != TokenType::RPAREN) throw std::runtime_error("Missing closing parenthesis");
            advance();
            return val;
        }
        throw std::runtime_error("Unexpected token: " + current.value);
    }

    double callFunction(const std::string& func, double arg) {
        if (func == "sin") return std::sin(arg);
        if (func == "cos") return std::cos(arg);
        if (func == "tan") return std::tan(arg);
        if (func == "sqrt") return std::sqrt(arg);
        if (func == "log") return std::log(arg);
        if (func == "exp") return std::exp(arg);
        if (func == "abs") return std::fabs(arg);
        if (func == "deg") return arg * (M_PI / 180.0);
        throw std::runtime_error("Unknown function: " + func);
    }
};

double evaluateExpression(const std::string& expr) {
    try {
        Parser parser(expr);
        return parser.parse();
    } catch (const std::exception& e) {
        // Instead of crashing, print error or log it
        // std::cerr << "Parse error: " << e.what() << std::endl;
        return std::nan(""); // or some sentinel value
    }
}

bool isMathValidExpression(const std::string& expr) {
    // Quick sanity: if empty or no digits, false
    bool hasDigit = false;
    for (char c : expr) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            hasDigit = true;
            break;
        }
    }
    if (!hasDigit) return false;

    // Allowed characters
    for (char c : expr) {
        if (!std::isspace(static_cast<unsigned char>(c)) &&
            !std::isdigit(static_cast<unsigned char>(c)) &&
            !std::isalpha(static_cast<unsigned char>(c)) &&
            c != '.' && c != '(' && c != ')' &&
            c != '+' && c != '-' && c != '*' && c != '/' && c != '^') {
            return false;
        }
    }

    // Check balanced parentheses
    int depth = 0;
    for (char c : expr) {
        if (c == '(') depth++;
        else if (c == ')') depth--;
        if (depth < 0) return false;
    }
    if (depth != 0) return false;

    // Trim trailing whitespace
    int lastNonSpaceIndex = -1;
    for (int i = (int)expr.size() - 1; i >= 0; --i) {
        if (!std::isspace(static_cast<unsigned char>(expr[i]))) {
            lastNonSpaceIndex = i;
            break;
        }
    }
    if (lastNonSpaceIndex == -1) return false; // empty or only spaces

    char lastChar = expr[lastNonSpaceIndex];

    // Expression must NOT end with an operator or '('
    if (lastChar == '+' || lastChar == '-' || lastChar == '*' ||
        lastChar == '/' || lastChar == '^' || lastChar == '(') {
        return false;
    }

    // Check that operators are followed by valid operands
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
            size_t j = i + 1;
            while (j < expr.size() && std::isspace(static_cast<unsigned char>(expr[j]))) j++;

            if (j == expr.size()) {
                return false;
            }

            char nextChar = expr[j];
            if (!(std::isdigit(static_cast<unsigned char>(nextChar)) ||
                  std::isalpha(static_cast<unsigned char>(nextChar)) ||
                  nextChar == '.' || nextChar == '(')) {
                return false;
            }
        }
    }

    return true;
}
