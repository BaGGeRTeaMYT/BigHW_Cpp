// #include <StringCell.h>

// StringCell::StringCell( size_t size ): Cell(true, STRING_TYPE), m_size(size) {
// }

// StringCell::StringCell( std::string value, size_t size ): Cell(false, STRING_TYPE), m_value(value), m_size(size) {
//     if (value.size() > size) {
//         throw std::runtime_error("Trying to fit string with greater length then expected\n");
//     }
// }

// const cell_type StringCell::get_type( void ) const {
//     return m_type;
// }

// void StringCell::set_value( std::string value ) {
//     if (value.size() > m_size) {
//         throw std::runtime_error("Trying to fit string with greater length then expected\n");
//     }
//     m_is_null = false;
//     m_value = value;
// }

// std::pair<bool, std::string> StringCell::get_value( void ) const {
//     return {m_is_null, m_value};
// }

// size_t StringCell::get_size( void ) const {
//     return m_size;
// }