#include <not_sql>

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
        "( shaurma : string[2]);"
        
        "iNsErt ("
            "col1= 123,"
            "col3=false,"
            "col4    =        true"
        ") tO table_name;"

        "iNsErt ("
            "col2 = 0x12,"
            "col3=false,"
            "col4    =        true"
        ") tO table_name;"

        "iNsErt ("
            "col1 =  1234,"
            "col2 = 0x132,"
            "col3=false,"
            "col4    =        true"
        ") tO table_name;"
        "INSERT (shaurma = \"ab\") to uzbeki_klass;"
        
        "select * from table_name where true;"
        "delete table_name where col1 > 1000;"
        "select * from table_name where true;"
        "update table_name "
        "set col1 = col1*2 + 2 "
        "where true;"
        "select * from table_name where true;"
    ));

    return 0;
}