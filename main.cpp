#include <iostream>
#include <Query.h>
#include <Table.h>

int main() {
    // should begin with create table
    Query create_test = Query(
        std::string(
            
            // "create tABle table_name"
            //     "("
            //     "{attr1, attr2,attr3}"
            //     "col1 : int32 = 31,"
            //     "{attr2,attr3}  "
            //     "col2: bytes[2] = 0x1234,"
            //     "{} col3    : bool = true,"
            //     "col4    : bool"
            // ");"
            
            // "iNsErt ("
            //     "col1= 123,"
            //     "col2 =0x0456"
            // ") tO table_name;"
            
            // "seLeCt col1,col2 "
            // "fRom table_name "
            // "whERe col1 < 23 && (true || false);"
            
            // "UPDATe table_name "
            // "sEt col1 =321 + col1*2,"
            // "col2 = 0xb0ba ,   "
            // "col3 = \"uzbeki power\" + col3 "
            // "WHeRE col2 < 0x0500;"
            
            "DELETE table_name "
            "where col = 321 && col2 = 0x0456;"
        ));

    // new tests required

    // Table a("asd");
    // a.get_all_columns();

    return 0;
}