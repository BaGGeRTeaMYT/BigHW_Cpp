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
        throw std::runtime_error("Read until: error");
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
    for (from; from < str.size() && !isspace(str[from]) && str[from] != ',' && str[from] != '}' && str[from] != ')'; from++) {
        answer += str[from];
    }
    if constexpr (save_pos) {
        from = save_from;
    }
    return answer;
}

OperationCreate::OperationCreate(const std::string& args) {
    if (args.size() == 0) {
        std::cout << "Operation error:" << std::endl
                  << "CREATE TABLE <name> (column params);" << std::endl
                  << "(column params) sohould look exactly like this:" << std::endl
                  << "[{attribute1, attribute2,...}] <name>: <type> [ = <value>]" << std::endl
                  << "[...] for optional params" << std::endl
                  << "Arguments are missing." << std::endl;
        // TODO throw some error with this cute (:3 OwO UwU) msg
    }
    std::cout << "Parsing args: {" << args << "}" << std::endl;
    std::string tmp;
    bool name_found = false;
    size_t idx = 0;
    skip_spaces(args, idx);
    table_name = read_until<' '>(args, idx); // TODO check if name is unique
    if (table_name[0] == '(') {
        std::cout << "CREATE TABLE: name is missing" << std::endl;
    }
    skip_spaces(args, idx);
    if (idx != args.size()) { // has arguments
        idx++;
        while (idx < args.size() && args[idx] != ')') {
            skip_spaces(args, idx);
            if (args[idx] == '{') { // attributes list
                std::vector<std::string> attributes;
                idx++;
                for (; args[idx] != '}';) {
                    skip_spaces(args, idx);
                    attributes.push_back( get_word(args, idx) );
                    skip_spaces(args, idx);
                    if (args[idx] == ',') {
                        idx++;
                    }
                }
                col_attributes.push_back(attributes);
                idx++;
            }
            skip_spaces(args, idx);
            std::string name = read_until<':'>(args, idx);
            remove_extra_spaces(name);
            col_name.push_back(name);
            idx++;

            skip_spaces(args, idx);
            std::string type;
            if (args[idx] == 'i') {
                type = read_until<'2'>(args, idx);
                idx++;
                type += "2";
                if (type != "int32") {
                    std::cout << "CREATE TABLE: " << type << " is not valid type name. May be you meant int32?" << std::endl;
                }
                col_type.push_back(type);
                col_sizeof.push_back(-1);
            } else if (args[idx] == 'b') { // bool or bytes

                if (idx == args.size() - 1) {
                    // nothing after b
                    std::cout << "CREATE TABLE: missing bracket, closing attribute params" << std::endl;
                
                } else if (args[idx + 1] == 'o') { // bool
                    type = read_until<'l'>(args, idx);
                    idx++;
                    type += "l";
                    if (type != "bool") {
                        std::cout << "CREATE TABLE: " << type << " is not valid type name. May be you meant bool?" << std::endl;
                    }
                    col_type.push_back(type);
                    col_sizeof.push_back(-1);

                } else if (args[idx + 1] == 'y') { // bytes
                    type = read_until<'s'>(args, idx);
                    idx++;
                    if (idx == args.size()) {
                        std::cout << "CREATE TABLE: missing bracket, closing attribute params" << std::endl;
                    }
                    type += "s";
                    if (type != "bytes") {
                        std::cout << "CREATE TABLE: " << type << " is not valid type name. May be you meant bytes[X]?" << std::endl;
                    }
                    col_type.push_back(type);
                    skip_spaces(args, idx);
                    if (args[idx] == '[') {
                        idx++;
                        std::string size_spec = read_until<']'>(args, idx);

                        col_sizeof.push_back(std::stoi(size_spec));
                        idx++;

                    } else {
                        std::cout << "CREATE TABLE: bytes type requires size specification." << std::endl
                                  << "e.g. bytes[1] or bytes[12345]" << std::endl;
                    }
                }

            } else if (args[idx] == 's') { // string
                type = read_until<'g'>(args, idx);
                idx++;
                if (idx == args.size()) {
                    std::cout << "CREATE TABLE: missing bracket, closing attribute params" << std::endl;
                }
                type += "g";
                if (type != "string") {
                    std::cout << "CREATE TABLE: " << type << " is not valid type name. May be you meant string[X]?" << std::endl;
                }
                col_type.push_back(type);
                skip_spaces(args, idx);
                if (args[idx] == '[') {
                    idx++;
                    std::string size_spec = read_until<']'>(args, idx);
                    // TODO check if there is actually number inside []
                    col_sizeof.push_back(std::stoi(size_spec));
                    idx++;
                } else {
                    std::cout << "CREATE TABLE: string type requires size specification." << std::endl
                              << "e.g. string[10] or string[10203]" << std::endl;
                }
                

            } else { // error
                std::cout << "CREATE TABLE: unable to recognize given column value type." << std::endl
                          << "Possible variants: int32, bool, string[X], bytes[X]" << std::endl;
            }
            skip_spaces(args, idx);
            if (args[idx] == '=') {
                idx++;
                skip_spaces(args, idx);
                // Update defaults for STRING
                // Should read inside " "
                col_default_value.push_back(get_word(args, idx));

            } else {
                col_default_value.push_back("");
            }
            skip_spaces(args, idx);
            if (args[idx] == ',') {
                idx++;
            }
        }
        std::cout << "Log: table " << table_name << " ready to be created" << std::endl;
        for (size_t i = 0; i < col_type.size(); i++) {
            std::cout << i << ": {";
            size_t cnt = 0;
            for (const auto& j : col_attributes[i]) {
                std::cout << j;
                if (col_attributes[i].begin() + cnt != col_attributes[i].end() - 1) {
                    std::cout << ", ";
                }
                cnt++;
            } 
            std::cout << "} " << col_name[i] << ": " << col_type[i];
            if (col_type[i] == "string" || col_type[i] == "bytes") {
                std::cout << "[" << col_sizeof[i] << "]";
            }
            if (col_default_value[i].size()) {
                std::cout << " = " << col_default_value[i];
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Log: empty table " << table_name << " ready to be created" << std::endl;
    }
}

Table OperationCreate::execute() {
    Table a;
    return a;
}