#include <IntCell.h>

IntCell::IntCell( void ): Cell(true, INT32_TYPE), m_value(0) {
}

IntCell::IntCell( int value ): Cell(false, INT32_TYPE), m_value(value) {
}

const cell_type IntCell::get_type( void ) const {
    return m_type;
}

void IntCell::set_value( int value ) {
    m_value = value;
    m_is_null = false;
}

std::pair<bool, int> IntCell::get_value( void ) const {
    return {m_is_null, m_value};
}