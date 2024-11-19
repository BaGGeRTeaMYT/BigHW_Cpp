#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <Cell.h>

class Column {
public:

    Column( std::string );

    void add_cell( int position, std::shared_ptr<Cell> cell);
    void remove_cell( int position );

    [[nodiscard]] std::shared_ptr<Cell>& get_cell( int position );
    [[nodiscard, noexcept]] std::vector< std::shared_ptr<Cell> >& get_all_cells( void );

private:

    std::string m_name;
    std::vector< std::shared_ptr<Cell> > m_cells;

};

#endif // COLUMN_H