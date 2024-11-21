#include <Cell.h>

Cell::Cell(bool is_null, cell_type type):
    m_is_null(is_null),
    m_type(type) {}

// string Cell

sCell::sCell( unsigned long size ):
    Cell(true, STRING_TYPE),
    m_value(std::string("")),
    m_capacity(size) {}

sCell::sCell(std::string& value, unsigned long size):
    Cell(false, STRING_TYPE),
    m_value(value),
    m_capacity(size) {}

const char sCell::get_type( void ) const {
    return STRING_TYPE;
}

// bytes Cell

byCell::byCell( unsigned long size ):
    Cell(true, BYTES_TYPE),
    m_value(std::vector<char>(size)),
    m_capacity(size) {}

byCell::byCell(std::vector<char>& value, unsigned long size):
    Cell(false, BYTES_TYPE),
    m_value(value),
    m_capacity(size) {}

const char byCell::get_type( void ) const {
    return BYTES_TYPE; 
}