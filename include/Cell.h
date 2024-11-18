#ifndef CELL_H
#define CELL_H

template <class T>
class Cell {
public:

    Cell( void );
    Cell( T value );

    [[nodiscard, noexcept]] T& get_value( void ) const;
    void set_value( T &value );

private:

    T m_value;
    bool m_is_null;

};

#endif // CELL_H