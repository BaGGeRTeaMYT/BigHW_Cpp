#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <Cell.h>

template <class T>
class Column {
public:

    Column( std::string );

    void add_cell( int position, std::shared_ptr<Cell<T>> cell);
    void remove_cell( int position );

    [[nodiscard]] std::shared_ptr<Cell<T>>& get_cell( int position ) const;
    [[nodiscard, noexcept]] std::vector< std::shared_ptr<Cell<T>> >& get_all_cells( void ) const;

private:

    std::string m_name;
    std::vector< std::shared_ptr<Cell<T>> > m_cells;

};

#endif // COLUMN_H