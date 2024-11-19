#ifndef CELL_H
#define CELL_H

#include <utility>
#include <vector>
#include <string>

#define INT32_TYPE 0x1
#define BOOL_TYPE 0x2
#define BYTES_TYPE 0x3
#define STRING_TYPE 0x4

class Cell {
public:

    Cell(bool is_null, char type);
    [[nodiscard]] virtual const char get_type( void ) const = 0; 

protected:

    bool m_is_null;
    char m_type;

};

class iCell : public Cell {
    iCell( void );
    iCell(int value);
    const char get_type( void ) const override;

    private:
    int m_value;
};

class boCell : public Cell {
    boCell( void );
    boCell(bool value);
    const char get_type( void ) const override;

    private:
    bool m_value;
};

class sCell : public Cell {
    sCell( unsigned long size );
    sCell(std::string& value, unsigned long size);
    const char get_type( void ) const override;

    private:
    std::string m_value;
    unsigned long m_capacity;
};

class byCell : public Cell {
    byCell( unsigned long size );
    byCell(std::vector<char>& value, unsigned long size);
    const char get_type( void ) const override;

    private:
    std::vector<char> m_value;
    unsigned long m_capacity;
};

#endif // CELL_H