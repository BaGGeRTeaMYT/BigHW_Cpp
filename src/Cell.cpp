#include <Cell.h>

Cell::Cell(bool is_null, cell_type type):
    m_is_null(is_null),
    m_type(type) {}


void Cell::set_null( bool state ) {
    m_is_null = state;
}

bool Cell::null_checker( void ) const {
    return m_is_null;
}

std::string hexation(bytes& vec) {
    std::string answer = "0x";
    std::reverse(vec.begin(), vec.end());
    for (std::byte b : vec) {
        char GO_F_YOURSELF_WITH_STD_BYTE = static_cast<char>(b);
        char first_sym = GO_F_YOURSELF_WITH_STD_BYTE >> 4;
        first_sym = first_sym > 9 ? (first_sym - 10 + 'a') : (first_sym + '0');
        char second_sym = GO_F_YOURSELF_WITH_STD_BYTE & 15;
        second_sym = second_sym > 9 ? (second_sym - 10 + 'a') : (second_sym + '0');
        answer += first_sym;
        answer += second_sym;
    }
    return answer;
}

std::ostream& operator<< (std::ostream& os, std::shared_ptr<Cell> cell) {
    std::shared_ptr<IntCell> try_int = std::dynamic_pointer_cast<IntCell>(cell);
    std::shared_ptr<BoolCell> try_bool = std::dynamic_pointer_cast<BoolCell>(cell);
    std::shared_ptr<BytesCell> try_bytes = std::dynamic_pointer_cast<BytesCell>(cell);
    std::shared_ptr<StringCell> try_string = std::dynamic_pointer_cast<StringCell>(cell);
    if (try_int != nullptr) {
        os << try_int->get_value().second;
    } else if (try_bool != nullptr) {
        os << (try_bool->get_value().second ? "true" : "false");
    } else if (try_bytes != nullptr) {
        os << hexation(try_bytes->get_value().second);
    } else if (try_string != nullptr) {
        os << try_string->get_value().second;
    } else {
        throw std::runtime_error("Something went completely wrong in printing a value of a cell.\n");
    }

    return os;
}


BoolCell::BoolCell( void ): Cell(true, BOOL_TYPE), m_value(0) {
}

BoolCell::BoolCell( bool value ): Cell(false, BOOL_TYPE), m_value(value) {
}

const cell_type BoolCell::get_type( void ) const {
    return m_type;
}

void BoolCell::set_value( bool value ) {
    m_is_null = false;
    m_value = value;
}

std::pair<bool, bool> BoolCell::get_value( void ) const {
    return {m_is_null, m_value};
}

BytesCell::BytesCell( size_t size ): Cell(true, BYTES_TYPE), m_size(0) {}

BytesCell::BytesCell( bytes value, size_t size ): Cell(false, BYTES_TYPE), m_value(value), m_size(value.size()) {
    if (value.size() > size) {
        throw std::runtime_error("Trying to fit bytes with greater length then expected\n");
    }
}

const cell_type BytesCell::get_type( void ) const {
    return m_type;
}

void BytesCell::set_value( bytes value ) {
    if (value.size() > m_size) {
        throw std::runtime_error("Trying to fit bytes with greater length then expected\n");
    }
    m_value = value;
    m_is_null = false;
}

std::pair<bool, bytes> BytesCell::get_value( void ) const {
    return {m_is_null, m_value};
}

size_t BytesCell::get_size( void ) const {
    return m_size;
}

IntCell::IntCell( void ): Cell(true, INT32_TYPE), m_value(0) {
}

IntCell::IntCell( int value ): Cell(false, INT32_TYPE), m_value(value) {
}

const cell_type IntCell::get_type( void ) const {
    return m_type;
}

void IntCell::set_value( int value ) {
    m_value = value;
    m_is_null = false;
}

std::pair<bool, int> IntCell::get_value( void ) const {
    return {m_is_null, m_value};
}

StringCell::StringCell( size_t size ): Cell(true, STRING_TYPE), m_size(size) {}

StringCell::StringCell( std::string value, size_t size ): Cell(false, STRING_TYPE), m_value(value), m_size(size) {
    if (value.size() > size) {
        throw std::runtime_error("Trying to fit string with greater length then expected\n");
    }
}

const cell_type StringCell::get_type( void ) const {
    return m_type;
}

void StringCell::set_value( std::string value ) {
    if (value.size() > m_size) {
        throw std::runtime_error("Trying to fit string with greater length then expected\n");
    }
    m_is_null = false;
    m_value = value;
}

std::pair<bool, std::string> StringCell::get_value( void ) const {
    return {m_is_null, m_value};
}

size_t StringCell::get_size( void ) const {
    return m_size;
}