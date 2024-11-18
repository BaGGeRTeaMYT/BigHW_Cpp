#include <Column.h>

template <class T>
Column<T>::Column( std::string name ): m_name(name), m_cells(0) {
}

template <class T>
void Column<T>::add_cell( int position, std::shared_ptr<Cell<T>> cell) {
    if (position > m_cells.size() || position < 0) {
        std::cerr << "Invalid position" << std::endl;
    }
    if (position == m_cells.size()) { m_cells.push_back(cell); }
    else { m_cells.insert(position, cell); }
}

template <class T> 
void remove_cell( int position ) {
    if (position >= m_cells.size() || position < 0) {
        std::cerr << "Invalid position" << std::endl;
    }
    m_cells.erase(m_cells.begin() + position);
}

template <class T>
[[nodiscard]] std::shared_ptr<Cell<T>>& get_cell( int position ) const {
    if (position >= m_cells.size() || position < 0) {
        std::cerr << "Invalid position" << std::endl;
    }
    return m_cells[position];
}

[[nodiscard, noexcept]] std::vector< std::shared_ptr<Cell<T>> >& get_all_cells( void ) const {
    return m_cells;
}