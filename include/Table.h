#ifndef TableHEADER
#define TableHEADER

#include <Column.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <stack>

typedef std::string table_name;
typedef std::shared_ptr<Column> column_pointer;
typedef std::map<const column_name, column_pointer> column_container;

enum class TokenType {
    INT32,
    BOOL,
    BYTES,
    STRING,
    OPERATOR,
    VARIABLE,
    LPAREN,
    RPAREN,
    SIZE,
    INVALID
};
struct Token {
    TokenType type;
    std::string value;
};

class Table {
public:

    Table( table_name name );

    void add_column( column_pointer column );
    void remove_column( const column_name& name );

    [[nodiscard]] table_name get_name( void ) const;
    [[nodiscard]] column_pointer get_column( const column_name& column_name );
    [[nodiscard]] column_container::const_iterator find_column( const column_name& name ) const;
    [[nodiscard]] const column_container& get_all_columns( void ) const;

    std::vector<size_t> apply_condition(const std::vector<Token>& tokens);

private:

    table_name m_name;
    column_container m_columns;

};

#endif // TableHEADER
