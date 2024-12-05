#ifndef TableHEADER
#define TableHEADER

#include <Column.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

#define hexation some_other_function_please_stop_linker_errors

typedef std::string table_name;
typedef std::shared_ptr<Column> column_pointer;
typedef std::map<const column_name, column_pointer> column_container;
typedef std::vector<std::shared_ptr<Cell>> row;

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

    [[nodiscard]] row get_row( int index ) const;
    void remove_row( int index );
    [[nodiscard]] int rows_count( void ) const;

    [[nodiscard]] table_name get_name( void ) const;
    [[nodiscard]] column_pointer get_column( const column_name& column_name ) const ;
    [[nodiscard]] column_container::const_iterator find_column( const column_name& name ) const;
    [[nodiscard]] const column_container& get_all_columns( void ) const;
    [[nodiscard]] std::stack<Token> calculate_expression( std::vector<Token> tokens, int row_index ) const;

    std::vector<size_t> apply_condition(const std::vector<Token>& processed_tokens) const;

private:

    table_name m_name;
    column_container m_columns;

};

#endif // TableHEADER
