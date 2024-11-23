#include <Column.h>

Column::Column( column_name name, cell_type type, attribute unique, attribute autoincrement, attribute key ): m_name(name), m_type(type), m_cells(0),
                                                                                                            m_attributes({unique, autoincrement, key})
                                                                                                            m_string_index('')
                                                                                                            m_bytes_index({0})
                                                                                                            m_bool_index(0)
                                                                                                            m_int_index(0) {
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
        for (auto i: m_cells) {
            if (i.get() == cell.get()) {
                throw std::runtime_error("Trying to add not unique cell\n");
            }
        }
        m_cells.push_back(cell);
    }
    if (m_attributes.autoincrement) {
        m_cells.push_back(m_cells.size() + 1);
    }
    if (m_attributes.key) {
        switch (m_type) {
            INT32_TYPE:
                break;
            BOOL_TYPE:
                break;
            BYTES_TYPE:
                break;
            STRING_TYPE:
                break;
            default:
                throw std::runtime_error("dafaq did your pass in the column\n");
        }
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