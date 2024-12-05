// #include <BytesCell.h>

// BytesCell::BytesCell( size_t size ): Cell(true, BYTES_TYPE), m_size(0) {
// }

// BytesCell::BytesCell( bytes value, size_t size ): Cell(false, BYTES_TYPE), m_value(value), m_size(value.size()) {
//     if (value.size() > size) {
//         throw std::runtime_error("Trying to fit bytes with greater length then expected\n");
//     }
// }

// const cell_type BytesCell::get_type( void ) const {
//     return m_type;
// }

// void BytesCell::set_value( bytes value ) {
//     if (value.size() > m_size) {
//         throw std::runtime_error("Trying to fit bytes with greater length then expected\n");
//     }
//     m_value = value;
//     m_is_null = false;
// }

// std::pair<bool, bytes> BytesCell::get_value( void ) const {
//     return {m_is_null, m_value};
// }

// size_t BytesCell::get_size( void ) const {
//     return m_size;
// }