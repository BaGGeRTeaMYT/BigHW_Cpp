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
