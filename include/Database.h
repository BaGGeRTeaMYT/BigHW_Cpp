#ifndef DatabaseHEADER
#define DatabaseHEADER

#include <vector>
#include <fstream>
#include <memory>
#include <string>
#include <Table.h>

class Database {
public:

    Database( void );

    void add_table( int position, std::shared_ptr<Table> table );
    void remove_table( int position );
    std::shared_ptr<Table> get_table( int position );
    
private:

    std::string m_name;
    std::vector< std::shared_ptr<Table> > m_tables;

};

#endif // DatabaseHEADER