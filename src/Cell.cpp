#include <Cell.h>

template <class T>
Cell<T>::Cell( void ): m_value(0), m_is_null(true) {
}

template <class T>
Cell<T>::Cell( T value ): m_value(value), m_is_null(false) {
}

template <class T>
void Cell<T>::set_value( T &value ) {

    m_value = value;
    m_is_null = false;

}

template <class T> 
[[nodiscard, noexcept]] T& Cell<T>::get_value( void ) const {

    if (m_is_null) { return NULL; }
    return &m_value;

}