#ifndef CELL_H
#define CELL_H

#include <utility>
#include <iostream>

#define INT32_TYPE 0x0
#define BOOL_TYPE 0x1
#define BYTES_TYPE 0x2
#define STRING_TYPE 0x3

typedef char cell_type;

class Cell {
public:

    Cell(bool is_null, cell_type type);
    void set_null( bool state );
    [[nodiscard]] virtual const cell_type get_type( void ) const = 0;

protected:

    bool m_is_null;
    cell_type m_type;

};

#endif /* CELL_H */ 