#ifndef SAVER_H
#define SAVER_H

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

class Saver {
public:

    Saver( std::string file_name );

    void save_to_file( void );
    
private:

    std::ofstream m_file;

};

#endif // SAVER_H