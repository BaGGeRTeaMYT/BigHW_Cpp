#ifndef DatabaseHEADER
#define DatabaseHEADER

#include <map>
#include <fstream>
#include <memory>
#include <string>
#include <Table.h>

typedef std::shared_ptr<Table> table_pointer;
typedef std::map<std::string, std::shared_ptr<Table>> table_container;

class Database {
public:

    Database( std::string name );

    void add_table( table_pointer table );
    void remove_table( std::string name );
    table_pointer get_table( std::string name );
    
private:

    std::string m_name;
    table_container m_tables;

};

#endif // DatabaseHEADER