#include <StringCell.h>

StringCell::StringCell( void ): Cell(true, INT32_TYPE), m_value(0), m_size(0) {
}

StringCell::StringCell( std::string value ): Cell(false, INT32_TYPE), m_value(value), m_size(value.size()) {
}

cell_type StringCell::get_type( void ) const {
    if (m_is_null) { 
        std::cerr << "Can\'t get type of NULL cell" << std::endl;
    }
    return m_type;
}

void StringCell::set_value( std::string value ) {
    m_is_null = false;
    m_value = value;
    m_size = value.size();
}

std::string StringCell::get_value( void ) const {
    if (m_is_null) {
        std::cerr << "Can\'t get value of NULL cell" << std::endl;
    }
    return m_value;
}

size_t StringCell::get_size( void ) const {
    if (m_is_null) {
        std::cerr << "Can\'t get size of NULL cell" << std::endl;
    }
    return m_size;
}