#ifndef DatabaseHEADER
#define DatabaseHEADER

#include <map>
#include <map>
#include <fstream>
#include <memory>
#include <string>
#include <Table.h>
#include <Query.h>

typedef std::shared_ptr<Table> table_pointer;
typedef std::map<std::string, std::shared_ptr<Table>> table_container;

class Database {
public:

    Database( std::string name );

    void add_table( table_pointer table );
    void remove_table( std::string name );
    std::string get_name( void ) const;
    table_container::const_iterator find_table( const std::string& name ) const;
    table_pointer get_table( const std::string& name );
    const table_container& get_all_tables( void ) const;
    void execute( std::string query );
    static std::string hexation(std::byte number);

    friend class OperationCreate;
    
private:

    std::string m_name;
    table_container m_tables;

};

#endif // DatabaseHEADER