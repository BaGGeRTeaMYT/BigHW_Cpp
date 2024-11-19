#include <Column.h>

Column::Column( std::string name ): m_name(name), m_cells(0) {}

void Column::add_cell( int position, std::shared_ptr<Cell> cell) {
    if (position > m_cells.size() || position < 0) {
        std::cerr << "Invalid position" << std::endl;
    }
    if (position == m_cells.size()) { m_cells.push_back(cell); }
    else { m_cells.insert(m_cells.begin() + position, cell); }
}

void Column::remove_cell( int position ) {
    if (position >= m_cells.size() || position < 0) {
        std::cerr << "Invalid position" << std::endl;
    }
    m_cells.erase(m_cells.begin() + position);
}

[[nodiscard]] std::shared_ptr<Cell>& Column::get_cell( int position ) {
    if (position >= m_cells.size() || position < 0) {
        std::cerr << "Invalid position" << std::endl;
    }
    return m_cells[position];
}

[[nodiscard, noexcept]] std::vector< std::shared_ptr<Cell> >& Column::get_all_cells( void ) {
    return m_cells;
}