#ifndef INTCELL_H
#define INTCELL_H

#include <Cell.h>

class IntCell : public Cell {
public:

    IntCell( void );
    IntCell( int value );

    cell_type get_type( void ) const override;
    [[nodiscard]] std::pair<bool, int> get_value( void ) const;
    void set_value( int value );

private:

    int m_value;

};

#endif /* INTCELL_H */ 
