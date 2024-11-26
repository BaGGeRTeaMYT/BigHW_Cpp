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
        return ((leftType == TokenType::INT32 || leftType == TokenType::VARIABLE) &&
               (rightType == TokenType::INT32 || rightType == TokenType::VARIABLE)) ||
               ((leftType == TokenType::STRING || leftType == TokenType::VARIABLE) &&
               (rightType == TokenType::STRING || rightType == TokenType::VARIABLE));
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
                            } else {
                                params.push_back({});
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
                            } else {
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
                                tmp += "\"" + read_until<'\"'>(str, idx) + "\"" + " ";
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
                                tmp += get_word(str, idx) + " ";
                                skip_spaces(str, idx);
                            }
                            if (str[idx] == ',') {
                                idx++;
                                skip_spaces(str, idx);
                                break;
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

OperationCreate::OperationCreate(const std::vector<std::vector<std::string>>& args) {
    int cnt = -1;
    bool name_given = false;
    for (const auto& operands : args) {
        for (const auto& operand : operands) {
            if (!name_given) {
                table_name = operand;
                name_given = true;
            } else {
                if (cnt%3 == 0) {
                    col_attributes.push_back(operands);
                    break;
                } else if (cnt%3 == 1) {
                    col_name.push_back(operand);
                } else {
                    if (operand == *(operands.begin())) {
                        col_type.push_back(operand);
                    } else {
                        col_default_value.push_back(operand);
                    }
                }
            }
        }
        if (cnt%3 == 0 && operands.size() == 0) {
            col_attributes.push_back({});
        }
        if (cnt%3 == 2 && operands.size() == 1) {
            col_default_value.push_back({});
        }
        cnt++;
    }
    // std::cout << "Col_names:" << std::endl;
    // for (const auto& n : col_name) {
    //     std::cout << n << std::endl;
    // }
    // std::cout << std::endl;
    // std::cout << "Attribtues:" << std::endl;
    // for (const auto& x : col_attributes) {
    //     for (const auto& y: x) {
    //         std::cout << y << ' ';
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;

    // std::cout << "Types" << std::endl;
    // for (const auto& n : col_type) {
    //     std::cout << n << std::endl;
    // }
    // std::cout << std::endl;
    // std::cout << "Defaults to" << std::endl;
    // for (const auto& n : col_default_value) {
    //     std::cout << n << std::endl;
    // }

}

char byte_checker(char sym) {
    if (isdigit(sym)) {
        return 1;
    }
    if (('a' <= sym <= 'f') || ('A' <= sym <= 'F')) {
        return 2;
    }
    return 0; 
}

void OperationCreate::execute( Database& db ) {
    std::shared_ptr<Table> table = std::shared_ptr<Table>(new Table(table_name));
    for (int i = 0; i < col_name.size(); ++i) {
        int len = -1;
        if (col_type[i] != "int32" && col_type[i] != "bool") {
            std::regex numberRegex("\\[(\\d+)\\]");
            std::string::const_iterator searchStart(col_type[i].cbegin());
            std::smatch match;
            std::regex_search(searchStart, col_type[i].cend(), match, numberRegex);
            // std::cout << match.str() << std::endl;
            len = std::stoi(match.str().substr(1, match.str().size() - 2));
        }

        struct attr {
            bool unique;
            bool autoincrement;
            bool key;
        } tmp_attr;
        tmp_attr.unique = false;
        tmp_attr.autoincrement = false;
        tmp_attr.key = false;


        for (auto s: col_attributes[i]) {
            if (s == "unique") {
                tmp_attr.unique = true;
            }
            else if (s == "autoincrement") {
                tmp_attr.autoincrement = true;
            }
            else if (s == "key") {
                tmp_attr.key = true;
            }
            else {
                throw std::runtime_error("Trying to create column with invalid attribute");
            }
        }

        cell_type tmp_type = -1;

        if (col_type[i] == "int32") {
            tmp_type = INT32_TYPE;
        }
        else if (col_type[i] == "bool") {
            tmp_type = BOOL_TYPE;
        }
        else if (col_type[i][0] == 's') {
            tmp_type = STRING_TYPE;
        }
        else if (col_type[i][0] == 'b') {
            tmp_type = BYTES_TYPE;
        }
        else {
            throw std::runtime_error("Trying to create column with invalid column type\n");
        }

        std::shared_ptr<Column> col = std::make_shared<Column>(Column(col_name[i],
                                                                        tmp_type,
                                                                        len,
                                                                        tmp_attr.unique,
                                                                        tmp_attr.autoincrement,
                                                                        tmp_attr.key));
        switch (tmp_type) {
            case INT32_TYPE:
            {
                std::string def_val = col_default_value[i];
                int tmp_digit_cnt = std::count_if(def_val.begin(), def_val.end(), [](unsigned char c){ return std::isdigit(c); });
                if (tmp_digit_cnt != def_val.size()) {
                    throw std::runtime_error("Trying to pass non-int default value to int column\n");
                }
                col->set_defualt_value(std::stoi(def_val));
                break;
            }
            case BOOL_TYPE:
            {
                std::string bool_val = col_default_value[i];
                if (bool_val == "true") {
                    col->set_defualt_value(true);
                }
                else if (bool_val == "false") {
                    col->set_defualt_value(false);
                }
                else if (bool_val.size() == 0) {
                    col->set_defualt_value(false);
                }
                else {
                    throw std::runtime_error("Trying to pass non-bool default value to bool column\n");
                }
                break;
            }
            case STRING_TYPE:
            {
                std::string str_val = col_default_value[i];
                if (str_val[0] != '\"' || str_val.back() != '\"') {
                    throw std::runtime_error("String default value is incomplete\n");
                }
                col->set_defualt_value(str_val.substr(1, str_val.size() - 2));
                break;
            }
            case BYTES_TYPE:
            {
                std::string byte_val = col_default_value[i];
                if (byte_val[0] != '0' || byte_val[1] != 'x') {
                    throw std::runtime_error("Trying to pass non-hex default number to bytes column\n");
                }
                std::shared_ptr<bytes> tmp_bytes = std::make_shared<bytes>(bytes(0));
                unsigned int i;
                for (i = byte_val.size() - 1; i >= 2; i--) {
                    char result = byte_checker(byte_val[i]);
                    unsigned char value = 0;
                    if (result == 0) {
                        std::cout << "Incorrect symbol in bytes[X] literal: " << byte_val[i] << std::endl;
                        throw std::runtime_error("Incorrect symbol in bytes[X] literal.\n");
                    }
                    value += (byte_val[i] - ((result == 1) ? '0' : 'a')) << 4;
                    i--;
                    result = byte_checker(byte_val[i]);
                    if (result == 0) {
                        std::cout << "Incorrect symbol in bytes[X] literal: " << byte_val[i] << std::endl;
                        throw std::runtime_error("Incorrect symbol in bytes[X] literal.\n");    
                    }
                    value += (byte_val[i] - ((result == 1) ? '0' : 'a'));
                    tmp_bytes->push_back(std::byte(value));
                }
                // if byte_val.size() is odd
                if (byte_val.size()%2) {
                    char result = byte_checker(byte_val[2]);
                    unsigned char value = 0;
                    if (result == 0) {
                        std::cout << "Incorrect symbol in bytes[X] literal: " << byte_val[i] << std::endl;
                        throw std::runtime_error("Incorrect symbol in bytes[X] literal.\n");
                    }
                    value += (byte_val[i] - ((result == 1) ? '0' : 'a'));
                    tmp_bytes->push_back(std::byte(value));
                }
                col->set_defualt_value(tmp_bytes);
                break;
            }
            default:
            {
                throw std::runtime_error("Something happened to column type\n");
                break;
            }
        }
    }
    db.add_table(table);
}

OperationInsert::OperationInsert(const std::vector<std::vector<std::string>>& args) {
    table_name = args.back().front();
    for (const auto& operands : args) {
        if (operands == *(args.end() - 1)) {
            break;
        }
        if (operands.size() != 2) {
            std::cout << "Assignments in operation insert have to look like:" << std::endl
                      << "<column_name> = <new_vakue>" << std::endl;
            throw std::runtime_error("Wrong operation insert syntax.");
        }
        col_name.push_back(operands.front());
        new_value.push_back(operands.back());
    }
    std::cout << "Have to insert" << std::endl;
    for (size_t i = 0; i < col_name.size(); i++) {
        std::cout << col_name[i] << " = " << new_value[i] << std::endl;
    }
    std::cout << std::endl << "In table: " << table_name;

}

void OperationInsert::execute( Database& db ) {
    Table a("dummy_name");
}

OperationSelect::OperationSelect(const std::vector<std::vector<std::string>>& args) {
    std::cout << "Columns to select:" << std::endl;
    for (const auto& col_name : args[0]) {
        column_names.push_back(col_name);
        std::cout << col_name << ' ';
    }
    table_name = args[1][0];
    std::cout << std::endl << std::endl << "From table: " << std::endl << table_name << std::endl;
    std::cout << std::endl << "If condition met: " << std::endl;
    auto conditions = ExpressionParser(args[2][0]);
    m_condition = conditions.tokenize(conditions.get_actions());
    for (const auto &single_token : m_condition) {
        std::cout << "Expression " << single_token.value << " of type " << (int)single_token.type << std::endl;
    }
    std::cout << std::endl;
    
}   

void OperationSelect::execute(Database& db) {
    Table a("dummy_name");
}

OperationUpdate::OperationUpdate(const std::vector<std::vector<std::string>>& args) {
    bool name_given = false;
    for (const auto& operations : args) {
        if (!name_given) {
            table_name = operations[0];
            name_given = true;
        } else if (operations.size() > 1) {
            m_cols_to_assign.push_back(operations[0]);
            ExpressionParser polish_expr(operations[1]);
            m_assignments.push_back(polish_expr.tokenize(polish_expr.get_actions()));
        } else {
            ExpressionParser polish_expr(operations[0]);
            m_condition = polish_expr.tokenize(polish_expr.get_actions());
        }
    }
    
    for (int i = 0; i < m_assignments.size(); i++) {
        std::cout << "Assign to column " << m_cols_to_assign[i] << " expression: " << std::endl; 
        for (int j = 0; j < m_assignments[i].size(); j++) {
            std::cout << m_assignments[i][j].value << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "If met condition:" << std::endl;
    for (int i = 0; i < m_condition.size(); i++) {
        std::cout << m_condition[i].value << " ";
    }
    std::cout << std::endl;
}

void OperationUpdate::execute( Database& db ) {
    Table a("dummy_name");
}

OperationDelete::OperationDelete(const std::vector<std::vector<std::string>>& args) {
    bool name_given = false;
    for (const auto& operations : args) {
        if (!name_given) {
            table_name = operations[0];
            name_given = true;
        } else {
            ExpressionParser polish_expr(operations[0]);
            m_condition = polish_expr.tokenize(polish_expr.get_actions());
        }
    }
    std::cout << "Delete row from table: " << table_name << std::endl;
    std::cout << "If met condition:" << std::endl;
    for (int i = 0; i < m_condition.size(); i++) {
        std::cout << m_condition[i].value << " ";
    }
    std::cout << std::endl;
}

void OperationDelete::execute(Database& db) {
    Table a("dummy_name");
}

Query::Query(const std::string& str) {
    size_t idx = 0;
    std::string command;
    op_str = get_keywords(str, idx);
    compile();
    // for (auto& [j, t] : op_str) {
    //     std::cout << "Operation type = " << static_cast<int>(t) << std::endl;
    //     for (auto& k : j) {
    //         for (auto& a : k) {
    //             std::cout << a << ' ';
    //         }
    //         std::cout << std::endl;
    //     }
    //     std::cout << std::endl;
    // }
}

void Query::compile() {
    std::shared_ptr<Operation> tmp;
    for (const auto &operations : op_str) {
        switch (operations.second) {
            case CREATE_TABLE: {
                tmp = std::make_shared<OperationCreate>(OperationCreate(operations.first));
                break;
            }
            case INSERT: {
                tmp = std::make_shared<OperationInsert>(OperationInsert(operations.first));
                break;
            }
            case SELECT: {
                tmp = std::make_shared<OperationSelect>(OperationSelect(operations.first));
                break;
            }
            case UPDATE: {
                tmp = std::make_shared<OperationUpdate>(OperationUpdate(operations.first));
                break;
            }
            case DELETE: {
                tmp = std::make_shared<OperationDelete>(OperationDelete(operations.first));
                break;
            }
            /*
            case JOIN {
                
                break;
            }
            case CREATE_U_INDEX {
            
                break;
            }
            case CREATE_O_INDEX {
            
                break;
            }
            */
        }
        ops.push_back(tmp);
    }
    return;
}

const std::vector<std::shared_ptr<Operation>>& Query::get_ops() const {
    return ops;
}
