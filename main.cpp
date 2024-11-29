#include <iostream>
#include <Database.h>

int main() {
    // should begin with create table
    Database db("Uzbek");
    db.execute(std::string(
            
            "create tABle table_name"
                "("
                "{key, autoincrement}"
                "col1 : int32 = 31,"
                "{unique}  "
                "col2: bytes[2] = 0x1234,"
                "{} col3    : bool = true,"
                "col4    : bool"
            ");"

            "create table uzbeki_klass"
            "(shaurma : string[2])"
            
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
            
            // "DELETE table_name "
            // "where col = 321 && col2 = 0x0456;"
        ));

    for (const auto &[k, v] : db.get_all_tables()) {
        std::cout << k << std::endl;
    }

    // new tests required

    // Table a("asd");
    // a.get_all_columns();


    return 0;
}