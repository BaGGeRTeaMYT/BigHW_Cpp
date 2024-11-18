#ifndef LOADER_H
#define LOADER_H

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

class Loader {
public:

    Loader( std::string file_name );

    void load_from_file( void );
    
private:

    std::ifstream m_file;

};

#endif // LOADER_H