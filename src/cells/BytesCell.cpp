#include <BytesCell.h>

BytesCell::BytesCell( void ): Cell(true, INT32_TYPE), m_value(0), m_size(0) {
}

BytesCell::BytesCell( bytes value ): Cell(false, INT32_TYPE), m_value(value), m_size(value.size()) {
}

cell_type BytesCell::get_type( void ) const {
    if (m_is_null) { 
        std::cerr << "Can\'t get type of NULL cell" << std::endl;
    }
    return m_type;
}

void BytesCell::set_value( bytes value ) {
    m_is_null = false;
    m_value = value;
    m_size = value.size();
}

bytes BytesCell::get_value( void ) const {
    if (m_is_null) {
        std::cerr << "Can\'t get value of NULL cell" << std::endl;
    }
    return m_value;
}

size_t BytesCell::get_size( void ) const {
    if (m_is_null) {
        std::cerr << "Can\'t get size of NULL cell" << std::endl;
    }
    return m_size;
}