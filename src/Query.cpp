#include <Query.h>

ExpressionParser::ExpressionParser(const std::string& expression) {
    m_actions = toPolishNotation(tokenize(expression));
}

const std::string& ExpressionParser::get_actions() const {
    return m_actions;
}

std::vector<Token> ExpressionParser::tokenize(const std::string& expression) {
    std::vector<Token> tokens;
    // regulat expressions are great thing!
    std::regex token_regex(R"(([a-zA-Z_][a-zA-Z0-9_]*)|(0x[0-9A-Fa-f]+)|(\d+)|(true|false)|(\|\w+\|)|([\+\-\*/%<>=!&|^\(\)]+)|(\"[^\"]*\"))");
    std::sregex_iterator iter(expression.begin(), expression.end(), token_regex);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string token_value = match.str();
        Token token;
        bool value_set = false;

        if (std::regex_match(token_value, std::regex(R"(true|false)"))) {
            token.type = TokenType::BOOL;
        } else if (std::regex_match(token_value, std::regex(R"(0x[0-9A-Fa-f]+)"))) {
            token.type = TokenType::BYTES;
        } else if (std::regex_match(token_value, std::regex(R"(\d+)"))) {
            token.type = TokenType::INT32;
        } else if (std::regex_match(token_value, std::regex(R"(\|\w+\|)"))) {
            if (token_value.front() == '|' && token_value.back() == '|') {
                token.type = TokenType::VARIABLE;
                token.value = token_value.substr(1, token_value.size() - 2); // Remove the '|' characters
                tokens.push_back(token);
                token.type = TokenType::OPERATOR;
                token.value = "SIZE";
                value_set = true;
            } else {
                throw std::invalid_argument("Opening/Closing '|' is missing");
            }
        } else if (std::regex_match(token_value, std::regex(R"([a-zA-Z_][a-zA-Z0-9_]*)"))) {
            token.type = TokenType::VARIABLE;
        } else if (std::regex_match(token_value, std::regex(R"([\+\-\*/%<>=!&|^\(\)]+)"))) {
            if (token_value == "!(") {
                token.type = TokenType::OPERATOR;
                token.value = "!";
                tokens.push_back(token);
                token.type = TokenType::LPAREN;
                token.value = "(";
                value_set = true;
            } else if (token_value == "(") {
                token.type = TokenType::LPAREN;
            } else if (token_value == ")") {
                token.type = TokenType::RPAREN;
            } else if (token_value == "&&" || token_value == "||" || token_value == "^^") {
                token.type = TokenType::OPERATOR;
            } else {
                token.type = TokenType::OPERATOR;
            }
        } else if (std::regex_match(token_value, std::regex(R"(\"[^\"]*\")"))) {
            token.type = TokenType::STRING;
        } else {
            token.type = TokenType::INVALID;
        }

        if (!value_set) {
            token.value = token_value;
        }
        tokens.push_back(token);
    }

    return tokens;
}

int ExpressionParser::getPrecedence(const std::string& op) {
    if (op == "||" || op == "&&" || op == "^^") return 1;
    if (op == "=" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") return 2;
    if (op == "+" || op == "-") return 3;
    if (op == "*" || op == "/" || op == "%") return 4;
    if (op == "SIZE" || op == "!") return 5;
    return 0;
}

bool ExpressionParser::isValidOperation(const std::string& op, const TokenType& leftType, const TokenType& rightType) {
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        return (leftType == TokenType::INT32 && rightType == TokenType::INT32) ||
               (leftType == TokenType::VARIABLE && rightType == TokenType::INT32) ||
               (leftType == TokenType::INT32 && rightType == TokenType::VARIABLE) ||
               (leftType == TokenType::VARIABLE && rightType == TokenType::VARIABLE);
    }
    if (op == "<" || op == ">" || op == "=" || op == "<=" || op == ">=" || op == "!=") {
        return leftType == rightType ||
               leftType == TokenType::VARIABLE || rightType == TokenType::VARIABLE;
    }
    if (op == "&&" || op == "||" || op == "^^") {
        return (leftType == TokenType::BOOL || leftType == TokenType::VARIABLE) &&
               (rightType == TokenType::BOOL || rightType == TokenType::VARIABLE);
    }
    if (op == "SIZE") {
        return leftType == TokenType::VARIABLE
            || leftType == TokenType::STRING || leftType == TokenType::BYTES;
    }
    if (op == "!") {
        return leftType == TokenType::BOOL || leftType == TokenType::VARIABLE;
    }
    return false;
}

std::string ExpressionParser::toPolishNotation(const std::vector<Token>& tokens) {
    std::stack<std::string> output;
    std::stack<std::string> operators;
    std::unordered_map<std::string, TokenType> variableTypes;

    for (const auto& token : tokens) {
        if (token.type == TokenType::INT32 || token.type == TokenType::BOOL || token.type == TokenType::VARIABLE || token.type == TokenType::STRING || token.type == TokenType::BYTES) {
            output.push(token.value);
            if (token.type == TokenType::VARIABLE) {
                variableTypes[token.value] = token.type;
            }
        } else if (token.type == TokenType::OPERATOR) {
            while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(token.value)) {
                output.push(operators.top());
                operators.pop();
            }
            operators.push(token.value);
        } else if (token.type == TokenType::LPAREN) {
            operators.push(token.value);
        } else if (token.type == TokenType::RPAREN) {
            while (!operators.empty() && operators.top() != "(") {
                output.push(operators.top());
                operators.pop();
            }
            if (!operators.empty() && operators.top() == "(") {
                operators.pop();
            } else {
                throw std::invalid_argument("Mismatched parentheses");
            }
        } else {
            throw std::invalid_argument("Invalid token: " + token.value);
        }
    }

    while (!operators.empty()) {
        if (operators.top() == "(" || operators.top() == ")") {
            throw std::invalid_argument("Mismatched parentheses");
        }
        output.push(operators.top());
        operators.pop();
    }

    std::vector<std::string> polishNotation;
    while (!output.empty()) {
        polishNotation.push_back(output.top());
        output.pop();
    }
    std::reverse(polishNotation.begin(), polishNotation.end());

    // Validate the Polish notation
    std::stack<TokenType> typeStack;
    for (const auto& token : polishNotation) {
        if (variableTypes.count(token)) {
            typeStack.push(variableTypes[token]);
        } else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "%" ||
                   token == "<" || token == ">" || token == "=" || token == "<=" || token == ">=" || token == "!=" ||
                   token == "&&" || token == "||" || token == "^^" || token == "SIZE" || token == "!") {
            if (token == "SIZE" || token == "!") {
                if (typeStack.empty()) {
                    throw std::invalid_argument("Invalid expression: not enough operands for operator " + token);
                }
                TokenType varType = typeStack.top(); typeStack.pop();                    if (!isValidOperation(token, varType, TokenType::INVALID)) {
                throw std::invalid_argument("Invalid operation: " + token + " for type " + std::to_string(static_cast<int>(varType)));
                }
                if (token == "SIZE") {
                    typeStack.push(TokenType::INT32); // Push the result type back to the stack
                } else {
                    typeStack.push(TokenType::BOOL); // Push the result type back to the stack
                }
            } else {
                if (typeStack.size() < 2) {
                    throw std::invalid_argument("Invalid expression: not enough operands for operator " + token);
                }
                TokenType rightType = typeStack.top(); typeStack.pop();
                TokenType leftType = typeStack.top(); typeStack.pop();
                if (!isValidOperation(token, leftType, rightType)) {
                    throw std::invalid_argument("Invalid operation: " + token + " for types " + std::to_string(static_cast<int>(leftType)) + " and " + std::to_string(static_cast<int>(rightType)));
                }
                // Push the result type back to the stack
                if (token == "<" || token == ">" || token == "=" || token == "<=" || token == ">=" || token == "!=" ||
                    token == "&&" || token == "||" || token == "^^") {
                    typeStack.push(leftType == TokenType::VARIABLE || rightType == TokenType::VARIABLE ? TokenType::VARIABLE : TokenType::BOOL);
                } else {
                    typeStack.push(leftType == TokenType::VARIABLE || rightType == TokenType::VARIABLE ? TokenType::VARIABLE : TokenType::INT32);
                }
            }
        } else {
            // If the token is an operand (INT32, BOOL, VARIABLE, STRING, BYTES), push its type to the stack
            if (std::regex_match(token, std::regex(R"(\d+)"))) {
                typeStack.push(TokenType::INT32);
            } else if (token == "true" || token == "false") {
                typeStack.push(TokenType::BOOL);
            } else if (std::regex_match(token, std::regex(R"(\"[^\"]*\")"))) {
                typeStack.push(TokenType::STRING);
            } else if (std::regex_match(token, std::regex(R"(0x[0-9A-Fa-f]+)"))) {
                typeStack.push(TokenType::BYTES);
            } else {
                throw std::invalid_argument("Invalid token in Polish notation: " + token);
            }
        }
    }

    if (typeStack.size() != 1) {
        throw std::invalid_argument("Invalid expression: too many operands");
    }

    // Convert the Polish notation to a string
    std::stringstream ss;
    for (const auto& token : polishNotation) {
        ss << token << " ";
    }
    return ss.str();
}

void remove_extra_spaces(std::string& str) {
    while (str.back() == ' ') {
        str.pop_back();
    }
}

void to_lower(std::string& str) {
    for (size_t i = 0; i < str.size(); i++) {
        if ('A' <= str[i] && str[i] <= 'Z') {
            str[i] = str[i] - 'A' + 'a';
        }
    }
}

std::string to_lower_copy(std::string str) {
    for (size_t i = 0; i < str.size(); i++) {
        if ('A' <= str[i] && str[i] <= 'Z') {
            str[i] = str[i] - 'A' + 'a';
        }
    }
    return str;
}

inline void skip_spaces(const std::string& str, size_t& from) {
    while (from < str.size() && isspace(str[from])) {
        from++;
    }
}


template<char end_sym, bool save_pos = false>
std::string read_until(const std::string& str, size_t& from) {
    size_t from_save = from;
    std::string answer = "";

    for (; from < str.size() && str[from] != end_sym; from++) {
        answer += str[from];
    }

    if (from == str.size()) {
        std::cout << "read_until: Symbol \'" << end_sym << "\' not found" << std::endl;
        throw std::runtime_error("Read until error: end symbol not found.");
    }

    if constexpr (save_pos) {
        from = from_save;
    }
    return answer;
}

bool special_symbol(char sym) {
    if (sym != '(' && sym != ',' && sym != '}'
     && sym != ')' && sym !=';'  && sym != '{'
     && sym !=':' && sym != '=') {
        return false;
    }
    return true;
}

// @brief Reads everything before space symbol or ',' symbol
template <bool save_pos = false>
std::string get_word(const std::string& str, size_t& from) {
    std::string answer = "";
    size_t save_from = from;
    for (from; from < str.size() && !isspace(str[from]) && !special_symbol(str[from]); from++) {
        answer += str[from];
    }
    if (from == str.size()) {
        std::cout << "get_word: word is the end of the string" << std::endl;
        throw std::runtime_error("Get word: error");
    }
    if constexpr (save_pos) {
        from = save_from;
    }
    return answer;
}

bool check_command(std::string& word) {
    std::string l_word = to_lower_copy(word);
    if (   l_word == "create" || l_word == "insert"
        || l_word == "select" || l_word == "update"
        || l_word == "delete") {
        return true;
    }
    return false;
}

std::vector<std::pair<std::vector<std::vector<std::string>>, char>> get_keywords(const std::string& str, size_t& idx) {
    std::vector<std::pair<std::vector<std::vector<std::string>>, char>> answer;
    std::vector<std::vector<std::string>> params;
    std::vector<std::string> single_type_params;
    while (idx < str.size()) {
        // read operation
        skip_spaces(str, idx);
        std::string tmp = get_word(str, idx);
        char operation_type = -1;
        skip_spaces(str, idx);
        if (check_command(tmp)) {
            to_lower(tmp);
            if (tmp == "create") {
                std::string second_word = get_word(str, idx);
                to_lower(second_word);
                skip_spaces(str, idx);
                if (second_word == "table") {
                    operation_type = OP_TYPE::CREATE_TABLE;
                } else if (second_word == "unordered") {
                    second_word = get_word(str, idx);
                    to_lower(second_word);
                    if (second_word == "index") {
                        operation_type = OP_TYPE::CREATE_U_INDEX;
                    }
                } else if (second_word == "ordered") {
                    second_word = get_word(str, idx);
                    to_lower(second_word);
                    if (second_word == "index") {
                        operation_type = OP_TYPE::CREATE_O_INDEX;
                    }
                } else {
                    std::cout << "Query creation error." << std::endl
                              << "Unable to recognize operation " << tmp + " " + second_word << std::endl;
                    throw std::runtime_error("Query compilation error.");
                }
                skip_spaces(str, idx);
            } else {

                if (tmp[0] == 'i') {
                    operation_type = OP_TYPE::INSERT;
                } else if (tmp[0] == 's') {
                    operation_type = OP_TYPE::SELECT;
                } else if (tmp[0] == 'u') {
                    operation_type = OP_TYPE::UPDATE;
                } else if (tmp[0] == 'd') {
                    operation_type = OP_TYPE::DELETE;
                }

            }
        } else {
            std::string possible_join = get_word(str, idx);
            skip_spaces(str, idx);
            if (to_lower_copy(possible_join) == "join") {
                single_type_params.push_back(tmp); // first table name
                operation_type = OP_TYPE::JOIN;
            } else {
                std::cout << "Query creation error." << std::endl
                          << "Unable to recognize operation " << tmp << std::endl;
                throw std::runtime_error("Query compilation error.");
            }
        }
        // now we know what operation this is
        // operation must end with ';'
        // last operation in query may not to end with ';'
        while (idx < str.size() && str[idx] != ';') {
            switch (operation_type) {

                case CREATE_TABLE:
                    // read table name
                    single_type_params.push_back(get_word(str, idx));
                    skip_spaces(str, idx);
                    params.push_back(single_type_params);
                    single_type_params.clear();
                    // there would be vector of vectors for every column
                    if (str[idx] == '(') {
                        idx++;
                        while (idx < str.size() && str[idx] != ')') {
                            skip_spaces(str, idx);
                            if (str[idx] == '{') {
                                idx++;
                                while (idx < str.size() && str[idx] != '}') {
                                    skip_spaces(str, idx);
                                    tmp = get_word(str, idx);
                                    if (tmp.size() > 0) {
                                        to_lower(tmp);
                                        single_type_params.push_back(tmp);
                                    }
                                    skip_spaces(str, idx);
                                    if (str[idx] == ',') {
                                        idx++;
                                    }
                                }
                                if (idx == str.size()) {
                                    std::cout << "In create table operation: closing '}' missing" << std::endl;
                                    throw std::runtime_error("In create table operation: closing '}' missing");
                                }
                                // to skip closing '}'
                                idx++;
                                // can push_back empty vector.
                                // It means columns has no attributes.
                                params.push_back(single_type_params);
                                single_type_params.clear();
                            }
                            skip_spaces(str, idx);
                            tmp = get_word(str, idx);
                            single_type_params.push_back(tmp);
                            // column name
                            params.push_back(single_type_params);
                            single_type_params.clear();
                            skip_spaces(str, idx);
                            if (str[idx] != ':') {
                                std::cout << "Error in create table operation:" << std::endl
                                          << "You should write ':' between column name and column type." << std::endl
                                          << "Column name is one word without spaces" << std::endl;
                                throw std::runtime_error("Error in create table operation");
                            }
                            idx++;
                            skip_spaces(str, idx);
                            tmp = get_word(str, idx);
                            to_lower(tmp);
                            bool str_flag = false;
                            if (tmp == "string") {
                                str_flag = true;
                            }
                            single_type_params.push_back(tmp);
                            skip_spaces(str, idx);
                            if (str[idx] == '[') {
                                idx++;
                                tmp = get_word(str, idx);
                                if (str[idx] != ']') {
                                    throw std::runtime_error("In create table operation: closing ']' is missing.");
                                }
                                single_type_params.push_back(tmp);
                                skip_spaces(str, idx);
                            }
                            if (str[idx] == '=') {
                                idx++;
                                skip_spaces(str, idx);
                                if (str_flag) { // default string values should be inside ""
                                    if (str[idx] == '\"') {
                                        idx++;
                                        tmp = read_until<'\"'>(str, idx);
                                        idx++;
                                    } else {
                                        std::cout << "String literal should be inside double quotes (\" \")." << std::endl;
                                        throw std::runtime_error("String literal should be inside double quotes (\" \").");
                                    }
                                } else {
                                    tmp = get_word(str, idx);
                                }
                                single_type_params.push_back(tmp);
                                skip_spaces(str, idx);
                                params.push_back(single_type_params);
                                single_type_params.clear();
                            }
                            if (str[idx] == ',') {
                                idx++;
                            }
                        }
                        if (idx == str.size()) {
                            std::cout << "In operation create table: closing ')' is missing" << std::endl;
                            throw std::runtime_error("In operation create table: closing ')' is missing");
                        }
                        idx++;
                    }
                    break;
                
                case INSERT:
                    skip_spaces(str, idx);
                    if (str[idx] == '(') {
                        // to skip '('
                        idx++;
                        while (idx < str.size() && str[idx] != ')') {
                            tmp = get_word(str, idx);
                            single_type_params.push_back(tmp);
                            skip_spaces(str, idx);
                            if (str[idx] != '=') {
                                std::cout << "Insert params should look like:" << std::endl
                                          <<"<column name> = <value>" << std::endl;
                                throw std::runtime_error("Insert parametrs format error.");
                            }
                            idx++;
                            skip_spaces(str, idx);
                            if (str[idx] == '\"') {
                                tmp = read_until<'\"'>(str, idx);
                                idx++;
                            } else {
                                tmp = get_word(str, idx);
                            }
                            skip_spaces(str, idx);
                            single_type_params.push_back(tmp);
                            params.push_back(single_type_params);
                            single_type_params.clear();
                            if (str[idx] == ',') {
                                idx++;
                            }
                            skip_spaces(str, idx);
                        }
                        if (idx == str.size()) {
                            std::cout << "In insert operation: closing '}' is missing" << std::endl;
                            throw std::runtime_error("In insert operation: closing '}' is missing");
                        }
                        // to skip ')'
                        idx++;
                        skip_spaces(str, idx);
                        tmp = get_word(str, idx);
                        to_lower(tmp);
                        if (tmp != "to") { 
                            std::cout << "In insert operation: keyword \"to\" is missing" << std::endl
                                      << "Operation should look like this: insert (<values>) to <table>" << std::endl;
                            throw std::runtime_error("In insert operation: keyword \"to\" is missing");
                        }
                        skip_spaces(str, idx);
                        tmp = get_word(str, idx);
                        single_type_params.push_back(tmp);
                        params.push_back(single_type_params);
                        single_type_params.clear();
                        skip_spaces(str, idx);
                        if (idx < str.size() && str[idx] != ';') {
                            std::cout << "Operation insert have to end on table name";
                            throw std::runtime_error("Operation insert have to end on table name");
                        }
                    } else {
                        std::cout << "Insert operation requires parameters in brackets '()'." << std::endl;
                        throw std::runtime_error("Insert operation requires parameters in brackets '()'.");
                    }
                    break;

                case SELECT: {
                    skip_spaces(str, idx);
                    bool continue_flag = true;

                    while (idx < str.size() && continue_flag) {
                        continue_flag = false;
                        tmp = get_word(str, idx);
                        single_type_params.push_back(tmp);
                        if (str[idx] == ',') {
                            idx++;
                            continue_flag = true;
                        }
                    }
                    skip_spaces(str, idx);

                    if (idx == str.size()) {
                        std::cout << "Incomplete operation update" << std::endl;
                        throw std::runtime_error("Incomplete operation update");
                    }
                    // selected columns
                    params.push_back(single_type_params);
                    single_type_params.clear();

                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    skip_spaces(str, idx);
                    if (tmp != "from") {
                        std::cout << "In operation select: keyword from is missing." << std::endl
                                  << "select <columns> from <table> where <condition>" << std::endl;
                        throw std::runtime_error("In operation select: keyword from is missing.");
                    }

                    tmp = get_word(str, idx);
                    skip_spaces(str, idx);
                    single_type_params.push_back(tmp);
                    // name of the table to select from
                    params.push_back(single_type_params);
                    single_type_params.clear();

                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    skip_spaces(str, idx);
                    if (tmp != "where") {
                        std::cout << "In operation select: keyword from is missing." << std::endl
                                  << "select <columns> from <table> where <condition>" << std::endl;
                        throw std::runtime_error("In operation select: keyword from is missing.");
                    }
                    tmp = "";
                    while (idx < str.size() && str[idx] != ';') {
                        tmp += str[idx];
                        idx++;
                    }
                    single_type_params.push_back(tmp);
                    // condition
                    params.push_back(single_type_params);
                    single_type_params.clear();

                    break;
                }
                case UPDATE: {
                    skip_spaces(str, idx);
                    tmp = get_word(str, idx);
                    single_type_params.push_back(tmp);
                    // name of the table to update
                    params.push_back(single_type_params);
                    single_type_params.clear();
                    skip_spaces(str, idx);

                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    if (tmp != "set") {
                        std::cout << "In operation update: keyword set is missing" << std::endl
                                  << "update <table> set <assignments> where <condition>" << std::endl;
                        throw std::runtime_error("In operation update: keyword set is missing");
                    }
                    skip_spaces(str, idx);

                    tmp = "";
                    bool continue_flag = true;
                    while (idx < str.size() && continue_flag) {
                        tmp = read_until<'='>(str, idx);
                        remove_extra_spaces(tmp);
                        idx++;
                        skip_spaces(str, idx);
                        single_type_params.push_back(tmp);
                        tmp = "";
                        while (idx < str.size() && continue_flag) {
                            if (str[idx] == '\"') {
                                idx++;
                                if (idx == str.size()) {
                                    std::cout << "Missing closing quote in string literal" << std::endl;
                                    throw std::runtime_error("Missing closing quote in string literal");
                                }
                                tmp += "\"" + read_until<'\"'>(str, idx) + "\"";
                                idx++;
                                if (idx == str.size()) {
                                    std::cout << "Unfinished operation update" << std::endl;
                                    throw std::runtime_error("Unfinished operation update");
                                }
                                skip_spaces(str, idx);
                            } else {
                                // saves idx
                                std::string possible_where = get_word<true>(str, idx);
                                to_lower(possible_where);
                                if (possible_where == "where") {
                                    continue_flag = false;
                                    idx += possible_where.size();
                                    skip_spaces(str, idx);
                                    break;
                                }
                                tmp += get_word(str, idx);
                                skip_spaces(str, idx);
                            }
                            if (str[idx] == ',') {
                                single_type_params.push_back(tmp);
                                params.push_back(single_type_params);
                                single_type_params.clear();
                            }
                        }
                        single_type_params.push_back(tmp);
                        // assignments
                        params.push_back(single_type_params);
                        single_type_params.clear();
                    }
                    if (idx == str.size()) {
                        std::cout << "In operation update: keyword \"where\" is missing." << std::endl
                                  << "update <table> set <assignments> where <condition>" << std::endl;
                        throw std::runtime_error("In operation update: keyword \"where\" is missing.");
                    }
                    tmp = "";
                    while (idx < str.size() && str[idx] != ';') {
                        tmp += str[idx];
                        idx++;
                    }
                    single_type_params.push_back(tmp);
                    // condition
                    params.push_back(single_type_params);
                    single_type_params.clear();
                    break;
                }
                case DELETE:
                    skip_spaces(str, idx);
                    tmp = get_word(str, idx);
                    single_type_params.push_back(tmp);
                    // name of the table to delete from
                    params.push_back(single_type_params);
                    single_type_params.clear();
                    skip_spaces(str, idx);

                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    skip_spaces(str, idx);
                    if (tmp != "where") {
                        std::cout << "In operation select: keyword from is missing." << std::endl
                                  << "select <columns> from <table> where <condition>" << std::endl;
                        throw std::runtime_error("In operation select: keyword from is missing.");
                    }
                    tmp = "";
                    while (idx < str.size() && str[idx] != ';') {
                        tmp += str[idx];
                        idx++;
                    }
                    single_type_params.push_back(tmp);
                    // condition
                    params.push_back(single_type_params);
                    single_type_params.clear();

                    break;

                case JOIN:
                    skip_spaces(str, idx);
                    tmp = get_word(str, idx);
                    single_type_params.push_back(tmp);
                    // second table name
                    params.push_back(single_type_params);
                    single_type_params.clear();

                    skip_spaces(str, idx);
                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    if (tmp != "on") {
                        std::cout << "In operation join: keyword \"on\" is missing." << std::endl
                                  << "<table1> join <table2> on <condition>" << std::endl;
                        throw std::runtime_error("In operation join: keyword \"on\" is missing.");
                    }
                    skip_spaces(str, idx);

                    tmp = "";
                    while (idx < str.size() && str[idx] != ';') {
                        tmp += str[idx];
                        idx++;
                    }
                    single_type_params.push_back(tmp);
                    // condition
                    params.push_back(single_type_params);
                    single_type_params.clear();

                    break;

                case CREATE_U_INDEX:
                    skip_spaces(str, idx);
                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    if (tmp != "on") {
                        std::cout << "In operation create unordered index: keyword \"on\" is missing." << std::endl
                                  << "create unordered index on <table> by <columns>" << std::endl;
                        throw std::runtime_error("In operation create unordered index: keyword \"on\" is missing.");
                    }
                    skip_spaces(str, idx);

                    tmp = get_word(str, idx);
                    single_type_params.push_back(tmp);
                    // table name
                    params.push_back(single_type_params);
                    single_type_params.clear();
                    skip_spaces(str, idx);

                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    if (tmp != "by") {
                        std::cout << "In operation create unordered index: keyword \"by\" is missing." << std::endl
                                  << "create unordered index on <table> by <columns>" << std::endl;
                        throw std::runtime_error("In operation create unordered index: keyword \"by\" is missing.");
                    }
                    skip_spaces(str, idx);

                    while (idx < str.size() && str[idx] != ';') {
                        tmp = get_word(str, idx);
                        single_type_params.push_back(tmp);
                        if (str[idx] == ',') {
                            idx++;
                        }
                        skip_spaces(str, idx);
                    }

                    params.push_back(single_type_params);
                    single_type_params.clear();
                    skip_spaces(str, idx);

                    break;

                case CREATE_O_INDEX:
                    skip_spaces(str, idx);
                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    if (tmp != "on") {
                        std::cout << "In operation create ordered index: keyword \"on\" is missing." << std::endl
                                  << "create ordered index on <table> by <columns>" << std::endl;
                        throw std::runtime_error("In operation create ordered index: keyword \"on\" is missing.");
                    }
                    skip_spaces(str, idx);

                    tmp = get_word(str, idx);
                    single_type_params.push_back(tmp);
                    // table name
                    params.push_back(single_type_params);
                    single_type_params.clear();
                    skip_spaces(str, idx);

                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    if (tmp != "by") {
                        std::cout << "In operation create ordered index: keyword \"by\" is missing." << std::endl
                                  << "create ordered index on <table> by <columns>" << std::endl;
                        throw std::runtime_error("In operation create ordered index: keyword \"by\" is missing.");
                    }
                    skip_spaces(str, idx);

                    while (idx < str.size() && str[idx] != ';') {
                        tmp = get_word(str, idx);
                        single_type_params.push_back(tmp);
                        if (str[idx] == ',') {
                            idx++;
                        }
                        skip_spaces(str, idx);
                    }

                    params.push_back(single_type_params);
                    single_type_params.clear();
                    skip_spaces(str, idx);
                    break;
            }
        }
        // to skip ';'
        idx++;
        answer.push_back(std::make_pair(params, operation_type));
        params.clear();
    }
    return answer;
}   

OperationCreate::OperationCreate(const std::string& args) {
    
}

Table OperationCreate::execute() {
    Table a;
    return a;
}

Query::Query(const std::string& str) {
    size_t idx = 0;
    std::string command;
    op_str = get_keywords(str, idx);
    for (auto& [j, t] : op_str) {
        std::cout << "Operation type = " << static_cast<int>(t) << std::endl;
        for (auto& k : j) {
            for (auto& a : k) {
                std::cout << a << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}