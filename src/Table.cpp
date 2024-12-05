#include <Table.h>

Table::Table( table_name name ): m_name(name), m_columns({}) {
}

table_name Table::get_name( void ) const {
    return m_name;
}

void Table::add_column( column_pointer column ) {
    int old_size = m_columns.size();
    m_columns[column->get_name()] = column;
    int new_size = m_columns.size();
    if (old_size == new_size) {
        throw std::runtime_error("Trying to add column with similar name to table\n");
    }
}

void Table::remove_column( const column_name& name ) {
    m_columns.erase(m_columns.find(name));
}

column_pointer Table::get_column( const column_name& name ) {
    return m_columns[name];
}

const column_container& Table::get_all_columns( void ) const {
    return m_columns;
}

column_container::const_iterator Table::find_column( const column_name& name ) const {
    return m_columns.find(name);
}

std::string hexation(bytes& vec) {
    std::string answer = "0x";
    for (std::byte b : vec) {
        char GO_F_YOURSELF_WITH_STD_BYTE = static_cast<char>(b);
        char first_sym = GO_F_YOURSELF_WITH_STD_BYTE >> 4;
        first_sym = first_sym > 9 ? (first_sym - 10 + 'a') : (first_sym - '0');
        char second_sym = GO_F_YOURSELF_WITH_STD_BYTE & 15;
        second_sym = second_sym > 9 ? (second_sym - 10 + 'a') : (second_sym - '0');
        answer += first_sym + second_sym;
    }
    return answer;
}

Token evaluate_operation(const Token& left, const Token& right, const std::string& op) {
    // if types mismatch we throw an error earlier
    if (left.type == TokenType::INT32) {
        int left_value = std::stoi(left.value);
        int right_value = std::stoi(right.value);
        int result_value;
        if (op == "+") {
            result_value = left_value + right_value;
        } else if (op == "-") {
            result_value = left_value - right_value;
        } else if (op == "*") {
            result_value = left_value * right_value;
        } else if (op == "/") {
            if (right_value == 0) {
                throw std::runtime_error("Expression you gave contains division by zero!\n");
            }
            result_value = left_value / right_value;
        } else if (op == "%") {
            if (right_value == 0) {
                throw std::runtime_error("Expression you gave contains division by zero!\n");
            }
            result_value = left_value % right_value;
        } else { // evaluations
            bool bool_result_value;
            if (op == "=") {
                bool_result_value = (left_value == right_value);
            } else if (op == ">") {
                bool_result_value = (left_value > right_value);
            } else if (op == "<") {
                bool_result_value = (left_value < right_value);
            } else if (op == ">=") {
                bool_result_value = (left_value >= right_value);
            } else if (op == "<=") {
                bool_result_value = (left_value <= right_value);
            } else if (op == "!=") {
                bool_result_value = (left_value != right_value);
            } else {
                std::string error_msg = "Unsupported operation " + op + " for INT32.\n";
                throw std::runtime_error(error_msg.c_str());
            }
            return {TokenType::BOOL, bool_result_value ? "true" : "false"};
        }
        return {TokenType::INT32, std::to_string(result_value)};
    } else if (left.type == TokenType::BOOL) {
        bool left_value = (left.value == "true");
        bool right_value = (right.value == "true");
        bool result_value;
        if (op == "&&") {
            result_value = left_value && right_value;
        } else if (op == "||") {
            result_value = left_value || right_value;
        } else if (op == "^^") {
            result_value = left_value ^ right_value;
        } else if (op == ">") {
            result_value = (left_value ? 1 : 0) > (right_value ? 1 : 0);
        } else if (op == ">=") {
            result_value = (left_value ? 1 : 0) >= (right_value ? 1 : 0);
        } else if (op == "<") {
            result_value = (left_value ? 1 : 0) < (right_value ? 1 : 0);
        } else if (op == "<=") {
            result_value = (left_value ? 1 : 0) <= (right_value ? 1 : 0);
        } else if (op == "!=") {
            result_value = (left_value ? 1 : 0) != (right_value ? 1 : 0);
        } else if (op == "=") {
            result_value = (left_value ? 1 : 0) == (right_value ? 1 : 0);
        } else {
            std::string error_msg = "Unsupported operation " + op + " for BOOL.\n";
            throw std::runtime_error(error_msg.c_str());
        }
        return {TokenType::BOOL, result_value ? "true" : "false"};
    } else if (left.type == TokenType::STRING) {
        std::string left_value = left.value;
        std::string right_value = right.value;
        std::string result_value;
        if (op == "+") {
            result_value = left_value + right_value;
        } else {
            bool bool_result_value;
            if (op == "=") {
                bool_result_value = (left_value == right_value);
            } else if (op == ">") {
                bool_result_value = (left_value > right_value);
            } else if (op == "<") {
                bool_result_value = (left_value < right_value);
            } else if (op == ">=") {
                bool_result_value = (left_value >= right_value);
            } else if (op == "<=") {
                bool_result_value = (left_value <= right_value);
            } else if (op == "!=") {
                bool_result_value = (left_value != right_value);
            } else {
                std::string error_msg = "Unsupported operation " + op + " for STRING.\n";
                throw std::runtime_error(error_msg.c_str());
            }
            return {TokenType::BOOL, bool_result_value ? "true" : "false"};
        }
        return {TokenType::STRING, result_value};
    } else if (left.type == TokenType::BYTES) {
        std::string left_value = left.value;
        std::string right_value = right.value;
        bool bool_result_value;
        if (op == "=") {
            bool_result_value = (left_value == right_value);
        } else if (op == ">") {
            bool_result_value = (left_value > right_value);
        } else if (op == "<") {
            bool_result_value = (left_value < right_value);
        } else if (op == ">=") {
            bool_result_value = (left_value >= right_value);
        } else if (op == "<=") {
            bool_result_value = (left_value <= right_value);
        } else if (op == "!=") {
            bool_result_value = (left_value != right_value);
        } else {
            std::string error_msg = "Unsupported operation " + op + " for BYTES.\n";
            throw std::runtime_error(error_msg.c_str());
        }
        return {TokenType::BOOL, bool_result_value ? "true" : "false"};
    } else {
        throw std::runtime_error("Unsupported token type (something is completely wrong).\n");
    }
}

std::vector<size_t> Table::apply_condition(const std::vector<Token>& tokens) {
    std::vector<size_t> result_indices;
    size_t row_count = m_columns.begin()->second->get_length();

    for (size_t i = 0; i < row_count; ++i) {
        std::vector<Token> processed_tokens = tokens;

        for (size_t j = 0; j < processed_tokens.size(); ++j) {
            if (processed_tokens[j].type == TokenType::VARIABLE) { // unwrapping variables
                const std::string& column_name = processed_tokens[j].value;
                auto requested_column = get_column(column_name);
                TokenType type = static_cast<TokenType>(requested_column->get_type());
                if (type != TokenType::STRING && type != TokenType::BYTES) {
                    // not unwrapping string and bytes because of possible operation SIZE
                    if (type == TokenType::INT32) {
                        processed_tokens[j].value = (std::dynamic_pointer_cast<IntCell>(requested_column->get_cell(i)))->get_value().second;
                    }
                    if (type == TokenType::BOOL) {
                        processed_tokens[j].value = (std::dynamic_pointer_cast<BoolCell>(requested_column->get_cell(i)))->get_value().second;
                    }
                    processed_tokens[j].type = type;
                }
            } else if (processed_tokens[j].type == TokenType::SIZE) {
                const std::string& column_name = processed_tokens[j - 1].value;
                auto requested_column = get_column(column_name);
                size_t size = requested_column->get_size();
                processed_tokens[j - 1].value = std::to_string(size);
                processed_tokens[j - 1].type = TokenType::INT32;
                processed_tokens.erase(processed_tokens.begin() + j);
                --j;
            }
        }

        // unwrapping all other variables, all SIZE operations should be gone by now
        for (size_t j = 0; j < processed_tokens.size(); ++j) {
            if (processed_tokens[j].type == TokenType::VARIABLE) {
                const std::string& column_name = processed_tokens[j].value;
                auto requested_column = get_column(column_name);
                TokenType type = static_cast<TokenType>(requested_column->get_type());
                if (type == TokenType::STRING) {
                    processed_tokens[j].value = (std::dynamic_pointer_cast<StringCell>(requested_column->get_cell(i)))->get_value().second;
                }
                if (type == TokenType::BYTES) {
                    processed_tokens[j].value = hexation((std::dynamic_pointer_cast<BytesCell>(requested_column->get_cell(i)))->get_value().second);
                }
                processed_tokens[j].type = type;
            }
        }

        std::stack<Token> stack;
        for (const auto& token : processed_tokens) {
            if (token.type == TokenType::OPERATOR) {
                if (token.value == "!") {
                    if (stack.size() < 1) {
                        throw std::runtime_error("Operation \"!\" should take at least one argument, none given.");
                    }
                } else if (stack.size() < 2) {
                    std::string error_msg = "Operation \"" + token.value + "\" should take at least two arguments, " + std::to_string(stack.size()) + " given.";
                    throw std::runtime_error(error_msg.c_str());
                    Token right = stack.top();
                    stack.pop();
                    Token left = stack.top();
                    stack.pop();
                    if (left.type != right.type) {
                        throw std::runtime_error("Type mismatch in operation");
                    }
                    Token result = evaluate_operation(left, right, token.value);
                    stack.push(result);
                }
            } else {
                stack.push(token);
            }
        }

        if (stack.size() != 1) {
            throw std::runtime_error("Invalid expression");
        }

        Token result = stack.top();
        if (result.type != TokenType::BOOL) {
            throw std::runtime_error("Condition should be of BOOL type.\n");
        }
        if (result.value == "true") {
            result_indices.push_back(i);
        }
    }

    return result_indices;
}

