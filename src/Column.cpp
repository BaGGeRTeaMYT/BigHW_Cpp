#include <Column.h>

Column::Column( column_name name,
                cell_type type,
                int length,
                attribute unique,
                attribute autoincrement,
                attribute key ):
                m_name(name),
                m_type(type),
                m_cells({}),
                m_length(length),
                m_attributes({unique, autoincrement, key}) {
    if (key) {
        m_attributes.unique = true;
    }
    if (autoincrement && type != INT32_TYPE) {
        throw std::runtime_error("Can\'t create non intager autoincrement column\n");
    }
}

void Column::add_cell( cell_pointer cell ) {
    if (cell->get_type() != m_type) {
        throw std::runtime_error("Trying to add invalid type cell to column\n");
    }
    if (cell->get_type() == STRING_TYPE) {
        int cell_len = std::dynamic_pointer_cast<StringCell>(cell)->get_size();
        if (cell_len != m_length) {
            throw std::runtime_error("Trying to add invalid length string cell to column\n");
        }
    }
    if (cell->get_type() == BYTES_TYPE) {
        int cell_len = std::dynamic_pointer_cast<BytesCell>(cell)->get_size();
        if (cell_len > m_length) {
            throw std::runtime_error("Trying to add invalid length bytes cell to column\n");
        }
    }
    if (m_attributes.unique) {
        switch (m_type) {
            case INT32_TYPE:
                for (auto i: m_cells) {
                    if (std::dynamic_pointer_cast<IntCell>(i)->get_value().second == 
                        std::dynamic_pointer_cast<IntCell>(cell)->get_value().second) {
                            throw std::runtime_error("Trying to insert not unique cell in unique column\n");
                    }
                }
                break;
            case BOOL_TYPE:
                for (auto i: m_cells) {
                    if (std::dynamic_pointer_cast<BoolCell>(i)->get_value().second == 
                        std::dynamic_pointer_cast<BoolCell>(cell)->get_value().second) {
                            throw std::runtime_error("Trying to insert not unique cell in unique column\n");
                    }
                }
                break;
            case BYTES_TYPE:
                for (auto i: m_cells) {
                    if (std::dynamic_pointer_cast<BytesCell>(i)->get_value().second == 
                        std::dynamic_pointer_cast<BytesCell>(cell)->get_value().second) {
                            throw std::runtime_error("Trying to insert not unique cell in unique column\n");
                    }
                }
                break;
            case STRING_TYPE:
                for (auto i: m_cells) {
                    if (std::dynamic_pointer_cast<StringCell>(i)->get_value().second == 
                        std::dynamic_pointer_cast<StringCell>(cell)->get_value().second) {
                            throw std::runtime_error("Trying to insert not unique cell in unique column\n");
                    }
                }
                break;
            default:
                throw std::runtime_error("Something has happened to column type\n");
                break;
            }
    }
    if (m_attributes.key) {
        switch (m_type) {
            case INT32_TYPE:
                m_int_index.insert(std::dynamic_pointer_cast<IntCell>(cell)->get_value().second);
                break;
            case BOOL_TYPE:
                m_bool_index.insert(std::dynamic_pointer_cast<BoolCell>(cell)->get_value().second);
                break;
            case BYTES_TYPE:
                m_bytes_index.insert(std::dynamic_pointer_cast<BytesCell>(cell)->get_value().second);
                break;
            case STRING_TYPE:
                m_string_index.insert(std::dynamic_pointer_cast<StringCell>(cell)->get_value().second);
                break;
            default:
                throw std::runtime_error("Something had happened to comuln type\n");
                break;
        }
    }
    if (m_attributes.autoincrement) {
        cell_pointer tmp_cell = std::shared_ptr<Cell>(new IntCell(std::dynamic_pointer_cast<IntCell>(m_cells[m_cells.size() - 1])->get_value().second + 1));
        m_cells.push_back(tmp_cell);
    }
    else {
        m_cells.push_back(cell);
    }
}

void Column::remove_cell( int position ) {
    if (position >= m_cells.size() || position < 0) {
        throw std::runtime_error("Invalid position\n");
    }
    if (m_attributes.key) {
        cell_pointer del_cell = get_cell(position);
        if (m_type == INT32_TYPE) {
            int val = std::dynamic_pointer_cast<IntCell>(del_cell)->get_value().second;
            m_int_index.erase(val);
        } else if (m_type == STRING_TYPE) {
            std::string val = std::dynamic_pointer_cast<StringCell>(del_cell)->get_value().second;
            m_string_index.erase(val);
        } else if (m_type == BOOL_TYPE) {
            bool val = std::dynamic_pointer_cast<BoolCell>(del_cell)->get_value().second;
            m_bool_index.erase(val);
        } else if (m_type == BYTES_TYPE) {
            bytes val = std::dynamic_pointer_cast<BytesCell>(del_cell)->get_value().second;
            m_bytes_index.erase(val);
        } else {
            throw std::runtime_error("Something has happened to column type\n");
        }
    }
    m_cells.erase(m_cells.begin() + position);
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

void Column::set_defualt_value( int value ) {
    if (m_type != INT32_TYPE) {
        throw std::runtime_error("Trying to set default int value to non-int column \n");
    }
    m_int_value = value;
}

void Column::set_defualt_value( bool value ) {
    if (m_type != BOOL_TYPE) {
        throw std::runtime_error("Trying to set default bool value to non-bool column \n");
    }
    m_bool_value = value;
}

void Column::set_defualt_value( const std::string& value ) {
    if (m_type != STRING_TYPE) {
        throw std::runtime_error("Trying to set default string value to non-string column \n");
    }
    if (value.size() != m_length) {
        throw std::runtime_error("Trying to set default string value to invalid length string\n");
    }
    m_string_value = value;
}

void Column::set_defualt_value( std::shared_ptr<bytes> value ) {
    if (m_type != BYTES_TYPE) {
        throw std::runtime_error("Trying to set default bytes value to non-bytes column \n");
    }
    if (value->size() > m_length) {
        throw std::runtime_error("Trying to set default bytes value to invalid length bytes\n");
    }
    m_bytes_value = value;
}
