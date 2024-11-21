#include <Table.h>

Table::Table( table_name name ): m_name(name), m_columns({}) {
}

table_name Table::get_name( void ) const {
    return m_name;
}

void Table::add_column( column_pointer column ) {
    m_columns[column->get_name()] = column;
}

void Table::remove_column( const column_name& name ) {
    m_columns.erase(m_columns.find(name));
}

column_pointer Table::get_column( const column_name& name ) const {
    return m_columns.at(name);
}

const column_container& Table::get_all_columns( void ) const {
    return m_columns;
}

column_container::const_iterator Table::find_column( const column_name& name ) const {
    return m_columns.find(name);
}