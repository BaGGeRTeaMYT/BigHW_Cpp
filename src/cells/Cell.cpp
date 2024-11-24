#include <Cell.h>

Cell::Cell(bool is_null, cell_type type):
    m_is_null(is_null),
    m_type(type) {}


void Cell::set_null( bool state ) {
    m_is_null = state;
}