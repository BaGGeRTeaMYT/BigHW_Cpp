#include <BoolCell.h>

BoolCell::BoolCell( void ): Cell(true, INT32_TYPE), m_value(0) {
}

BoolCell::BoolCell( bool value ): Cell(false, INT32_TYPE), m_value(value) {
}

cell_type BoolCell::get_type( void ) const {
    if (m_is_null) { 
        std::cerr << "Can\'t get type of NULL cell" << std::endl;
    }
    return m_type;
}

void BoolCell::set_value( bool value ) {
    m_is_null = false;
    m_value = value;
}

bool BoolCell::get_value( void ) const {
    if (m_is_null) {
        std::cerr << "Can\'t get value of NULL cell" << std::endl;
    }
    return m_value;
}