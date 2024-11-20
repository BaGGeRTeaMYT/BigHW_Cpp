#include <Query.h>

void remove_extra_spaces(std::string& str) {
    while (str.back() == ' ') {
        str.pop_back();
    }
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

// @brief Reads everything before space symbol or ',' symbol
template <bool save_pos = false>
std::string get_word(const std::string& str, size_t& from) {
    std::string answer = "";
    size_t save_from = from;
    for (from; from < str.size() && !isspace(str[from]) && str[from] != ',' && str[from] != '}' && str[from] != ')' && str[from] !=';'; from++) {
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
    while (idx < str.size()) {
        // read operation
        skip_spaces(str, idx);
        std::vector<std::string> single_type_params();
        std::string tmp = get_word(str, idx);
        char operation_type = -1;
        skip_spaces(str, idx);
        if (check_command(tmp)) {
            to_lower(tmp);
            if (tmp == "create") {
                std::string second_word = get_word(str, idx);
                to_lower(second_word);
                if (second_word == "table") {
                    operation_type = OP_TYPE::CREATE_TABLE;
                } else if (second_word == "index") {
                    operation_type = OP_TYPE::CREATE_INDEX;
                } else {
                    std::cout << "Query creation error." << std::endl
                              << "Unable to recognize operation " << tmp + " " + second_word << std::endl;
                    throw std::runtime_error("Query compilation error.");
                }
            }
            if (tmp[0] == 'i') {
                operation_type = OP_TYPE::INSERT;
            } else if (tmp[0] == 's') {
                operation_type = OP_TYPE::SELECT;
            } else if (tmp[0] == 'u') {
                operation_type = OP_TYPE::UPDATE;
            } else if (tmp[0] == 'd') {
                operation_type = OP_TYPE::DELETE;
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
                                idx++;
                                // can push_back empty vector.
                                // It means columns has no attributes.
                                params.push_back(single_type_params);
                                single_type_params.clear();
                            }
                            skip_spaces(str, idx);
                            tmp = get_word(str, idx);
                            // column name
                            single_type_params.push_back(tmp);
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
                    }
                    break;
                
                case INSERT:
                    skip_spaces(str, idx);
                    if (str[idx] == '(') {
                        while (idx < str.size() && str[idx] != ')') {
                            tmp = get_word(str, idx);
                            single_type_params.push_back(tmp);
                            skip_spaces(str, idx);
                            if (str[idx] != '=') {
                                std::cout << "Insert params should look like:" << std::endl
                                          <<"<column name> = <value>" << std::endl;
                                throw std::runtime_error("Insert parametrs format error.");
                            }
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
                        idx++;
                        skip_spaces(str, idx);
                        tmp = get_word(str, idx);
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
                        throw std::runtime_erorr("Insert operation requires parameters in brackets '()'.");
                    }
                    break;

                case SELECT:
                    skip_spaces(str, idx);
                    for (tmp = get_word(str, idx), skip_spaces(str, idx); str[idx] == ','; tmp = get_word(str, idx)) {
                        single_type_params.push_back(tmp);
                        skip_spaces(str, idx);
                    }
                    single_type_params.push_back(tmp);
                    skip_spaces(str, idx);
                    // selected columns
                    params.push_back(single_type_params);
                    single_type_params.clear();

                    tmp = get_word(str, idx);
                    to_lower(tmp);
                    skip_spaces(str, idx);
                    if (tmp != "from") {
                        std::cout << "In operation select: keyword from is missing." << std::endl;
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
                        std::cout << "In operation select: keyword from is missing." << std::endl;
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

                case UPDATE:
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
                    while (idx < str.size() && tmp != "where") {
                        tmp = read_until<'='>(str, idx);
                        remove_extra_spaces(tmp);
                        idx++;
                        skip_spaces(str, idx);
                        single_type_params.push_back(tmp);
                        tmp = ""
                        while (idx < str.size() && tmp != "where") {
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
                        std::cout << "In operation update: keyword from is missing." << std::endl;
                                  << "update <table> set <assignments> where <condition>" << std::endl;
                        throw std::runtime_error("In operation update: keyword from is missing.");
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
                        std::cout << "In operation select: keyword from is missing." << std::endl;
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

                    break;

                case CREATE_INDEX:

                    break;
            }
        }
        answer.push_back(std::make_pair(params, operation_type));
        params.clear();
    }
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
    auto ops = get_keywords(str, idx);
}