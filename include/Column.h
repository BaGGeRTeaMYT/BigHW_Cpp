#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <Cells.h>
#include <set>
#include <Types.h>

typedef std::string column_name;
typedef std::shared_ptr<Cell> cell_pointer;
typedef bool attribute;
typedef struct Attibutes {
    bool unique;
    bool autoincrement;
    bool key;
} attributes;
typedef std::set index;     

class Column {
public:

    Column( column_name name, cell_type type, 
        attribute unique = 0, attribute autoincrement = 0, attribute key = 0 );

    void add_cell( cell_pointer cell = nullptr );
    void remove_cell( int position );

    [[nodiscard, noexcept]] column_name get_name( void ) const;
    [[nodiscard]] cell_pointer& get_cell( int position );
    [[nodiscard, noexcept]] const std::vector<cell_pointer>& get_all_cells( void ) const;
    [[nodiscard, noexcept]] size_t get_size( void ) const;
    [[nodiscard, noexcept]] attributes& get_attributes( void ) const;

private:

    column_name m_name;
    cell_type m_type;
    std::vector<cell_pointer> m_cells;
    attributes m_attributes;
    index<std::string> m_string_index;
    index<bytes> m_bytes_index;
    index<bool> m_bool_index;
    index<int> m_int_index;
    
};

#endif // COLUMN_H