#ifndef BOOLCELL_H
#define BOOLCELL_H

#include <Cell.h>

class BoolCell : public Cell {
public:

    BoolCell( void );
    BoolCell( bool value );

    const char get_type( void ) const override;
    [[nodiscard]] bool get_value( void ) const;
    void set_value( bool value );

private:

    bool m_value;

};

#endif /* BOOLCELL_H */
