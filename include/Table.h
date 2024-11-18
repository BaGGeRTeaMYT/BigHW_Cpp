#ifndef TableHEADER
#define TableHEADER

#include <Column.h>
#include <string>
#include <memory>
#include <vector>

class Table {
public:

    Table( void );

    void add_column( int position, std::shared_ptr<Column> column );
    void remove_column( int position );
    std::shared_ptr<Column> get_column( std::string column_name );

private:

    std::string m_name;
    std::vector<Column> m_columns;

};

#endif // TableHEADER