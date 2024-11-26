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

table_pointer Database::get_table( std::string name ) const {
    return m_tables.at(name);
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
        } else {
            /* nothing here yet */
        }
    }
}
