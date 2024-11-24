#include <iostream>
#include <Query.h>

int main() {
    // should begin with create table
    std::cout << "Constructing create + insert" << std::endl;
    Query create_test =
    Query(
        std::string(
            // 
            // "create tABle table_name"
            //     "("
            //     "{attr1, attr2,attr3}"
            //     "col1 : int32 = 31,"
            //     "{attr2,attr3}  "
            //     "col2: bytes[2] = 0x1234,"
            //     "{} col3    : bool = true,"
            //     "col4    : bool"
            // ");"
            // 
            "iNsErt ("
                "col1= 123,"
                "col2 =0x0456"
            ") tO table_name;"
            //
            // "seLeCt col1,col2 "
            // "fRom table_name "
            // "whERe col1 < 23 && (true || false);"
            // 
            // "UPDATe table_name "
            // "sEt col1 =321 "
            // "WHeRE col2 < 0x0500;"
            // // 
            // "DELETE table_name "
            // "where col = 321 and col2 = 0x0456;"
        ));

    // new tests required
    return 0;
}