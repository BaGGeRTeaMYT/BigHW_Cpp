#include <Column.h>

Column::Column( column_name name, cell_type type, attribute unique, attribute autoincrement, attribute key ): m_name(name), m_type(type), m_cells(0),
                                                                                                            m_attributes({unique, autoincrement, key}) {
    if (key) {
        m_attributes.unique = true;
    }
    if (autoincrement and type != INT32_TYPE) {
        throw std::runtime_error("Can\'t create non intager autoincrement column\n");
    }
}

void Column::add_cell( cell_pointer cell ) {
    if (cell->get_type() != m_type) {
        throw std::runtime_error("Trying to add invalid type cell to column\n");
    }
    if (m_attributes.unique) {

    }
    if (m_attributes.autoincrement) {
        
    }
    
}

void Column::remove_cell( int position ) {
    if (position >= m_cells.size() || position < 0) {
        throw std::runtime_error("Invalid position\n");
    }
    m_cells[position]->set_null(true);
}

cell_pointer& Column::get_cell( int position ) {
    if (position >= m_cells.size() || position < 0) {
        throw std::runtime_error("Invalid position\n");
    }
    return m_cells[position];
}

column_name Column::get_name( void ) const {
    return m_name;
}

const std::vector<cell_pointer>& Column::get_all_cells( void ) const {
    return m_cells;
}

size_t Column::get_size( void ) const {
    return m_cells.size();
}