#ifndef STRINGCELL_H
#define STRINGCELL_H

#include <Cell.h>
#include <string>

class StringCell : public Cell {
public:

    StringCell( void );
    StringCell( std::string value );

    const char get_type( void ) const override;
    [[nodiscard]] std::string get_value( void ) const;
    [[nodiscard]] size_t get_size( void ) const;
    void set_value( std::string value );

private:

    std::string m_value;
    size_t m_size;

};

#endif /* STRINGCELL_H */
