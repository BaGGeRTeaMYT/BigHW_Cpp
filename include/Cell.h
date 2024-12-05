#ifndef CELL_H
#define CELL_H

#include <utility>
#include <iostream>
#include <memory>
#include <cstddef>
#include <vector>
#include <string>

typedef std::vector<std::byte> bytes;

#define INT32_TYPE 0x0
#define BOOL_TYPE 0x1
#define BYTES_TYPE 0x2
#define STRING_TYPE 0x3

typedef char cell_type;

class Cell {
public:

    Cell(bool is_null, cell_type type);
    void set_null( bool state );
    [[nodiscard]] virtual const cell_type get_type( void ) const = 0;
    bool null_checker( void ) const;
    friend std::ostream& operator<< (std::ostream& os, std::shared_ptr<Cell> cell);

protected:

    bool m_is_null;
    cell_type m_type;

};

class IntCell : public Cell {
public:

    IntCell( void );
    IntCell( int value );

    const cell_type get_type( void ) const override;
    [[nodiscard]] std::pair<bool, int> get_value( void ) const;
    void set_value( int value );

private:

    int m_value;

};

class StringCell : public Cell {
public:

    StringCell( size_t size );
    StringCell( std::string value, size_t size );

    const char get_type( void ) const override;
    [[nodiscard]] std::pair<bool, std::string> get_value( void ) const;
    [[nodiscard]] size_t get_size( void ) const;
    void set_value( std::string value );

private:

    std::string m_value;
    size_t m_size;

};

class BoolCell : public Cell {
public:

    BoolCell( void );
    BoolCell( bool value );

    const char get_type( void ) const override;
    [[nodiscard]] std::pair<bool, bool> get_value( void ) const;
    void set_value( bool value );

private:

    bool m_value;

};

class BytesCell : public Cell {
public:

    BytesCell( size_t size );
    BytesCell( bytes value, size_t size );

    const char get_type( void ) const override;
    [[nodiscard]] size_t get_size( void ) const;
    [[nodiscard]] std::pair<bool, bytes> get_value( void ) const;
    void set_value( bytes value );

private:

    bytes m_value;
    size_t m_size;

};

#endif /* CELL_H */ 