#ifndef BYTESCELL_H
#define BYTESCELL_H

#include <Cell.h>
#include <vector>
#include <cstddef>

typedef std::vector<std::byte> bytes;

class BytesCell : public Cell {
public:

    BytesCell( void );
    BytesCell( bytes value );

    const char get_type( void ) const override;
    [[nodiscard]] size_t get_size( void ) const;
    [[nodiscard]] bytes get_value( void ) const;
    void set_value( bytes value );

private:

    bytes m_value;
    size_t m_size;

};

#endif /* BYTESCELL_H */ 
