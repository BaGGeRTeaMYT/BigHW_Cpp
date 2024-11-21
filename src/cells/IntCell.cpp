#include <IntCell.h>

IntCell::IntCell( void ): Cell(true, INT32_TYPE), m_value(0) {
}

IntCell::IntCell( int value ): Cell(false, INT32_TYPE), m_value(value) {
}

cell_type IntCell::get_type( void ) const {
    if (m_is_null) { 
        std::cerr << "Can\'t get type of NULL cell" << std::endl;
    }
    return m_type;
}

void IntCell::set_value( int value ) {
    m_is_null = false;
    m_value = value;
}

int IntCell::get_value( void ) const {
    if (m_is_null) {
        std::cerr << "Can\'t get value of NULL cell" << std::endl;
    }
    return m_value;
}