#include <Database.h>

Database::Database( std::string name ): m_name(name), m_tables({}) {
}

void Database::add_table( table_pointer table ) {
    int old_size = m_tables.size();
    m_tables[table->get_name()] = table;
    int new_size = m_tables.size();
    if (new_size == old_size) {
        throw std::runtime_error("Trying to add table with existing name to database\n");
    }
}

void Database::remove_table( std::string name ) {
    m_tables.erase(m_tables.find(name));
}

const table_container& Database::get_all_tables( void ) const {
    return m_tables;
}

std::string Database::get_name( void ) const {
    return m_name;
}

table_pointer Database::get_table( const std::string& name ) {
    return m_tables[name];
}

table_container::const_iterator Database::find_table( const std::string& name) const {
    return m_tables.find(name);
}

void Database::execute( std::string query ) {
    Query q(query);
    auto ops = q.get_ops();
    for (auto i: ops) {
        if (std::dynamic_pointer_cast<OperationCreate>(i) != nullptr) {
            std::shared_ptr<OperationCreate> tmp = std::dynamic_pointer_cast<OperationCreate>(i);
            tmp->execute(*this);
        } else if (std::dynamic_pointer_cast<OperationInsert>(i) != nullptr){
            std::shared_ptr<OperationInsert> tmp = std::dynamic_pointer_cast<OperationInsert>(i);
            tmp->execute(*this);
        } else if (std::dynamic_pointer_cast<OperationDelete>(i) != nullptr){
            std::shared_ptr<OperationDelete> tmp = std::dynamic_pointer_cast<OperationDelete>(i);
            tmp->execute(*this);
        } else if (std::dynamic_pointer_cast<OperationUpdate>(i) != nullptr){
            std::shared_ptr<OperationUpdate> tmp = std::dynamic_pointer_cast<OperationUpdate>(i);
            tmp->execute(*this);
        } else if (std::dynamic_pointer_cast<OperationSelect>(i) != nullptr){
            std::shared_ptr<OperationSelect> tmp = std::dynamic_pointer_cast<OperationSelect>(i);
            tmp->execute(*this);
        } else if (std::dynamic_pointer_cast<OperationJoin>(i) != nullptr){
            std::shared_ptr<OperationJoin> tmp = std::dynamic_pointer_cast<OperationJoin>(i);
            tmp->execute(*this);
        } else {
            /* nothing here yet */
        }
    }
}

std::string Database::hexation(std::byte num) {
    unsigned char number = static_cast<unsigned char>(num);
    std::string answer = "";
    char first_symbol = static_cast<char>(number >> 4);
    if (first_symbol < 10) {
        first_symbol += '0';
    } else {
        first_symbol = first_symbol - 10 + 'a';
    }
    char second_symbol = static_cast<char>(number & 0xf);
    if (second_symbol < 10) {
        second_symbol += '0';
    } else {
        second_symbol = first_symbol - 10 + 'a';
    }
    answer += first_symbol;
    answer += second_symbol;
    return answer;
}