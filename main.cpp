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
            "( shaurma : string[2]);"
            
            "iNsErt ("
                "col1= 123,"
                "col2 =0x0456"
            ") tO table_name;"
            
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
        for (const auto& [col_k, col_v] : v->get_all_columns()) {
            std::cout << "\t" << col_k << ": ";
            for (int i = 0; i < col_v->get_size(); i++) {
                std::shared_ptr<IntCell> int_output = dynamic_pointer_cast<IntCell>(col_v->get_cell(i));
                if (int_output) {
                    std::cout << int_output->get_value().second << " ";
                }
                std::shared_ptr<BoolCell> bool_output = dynamic_pointer_cast<BoolCell>(col_v->get_cell(i));
                if (bool_output) {
                    std::cout << bool_output->get_value().second << " ";
                }
                std::shared_ptr<StringCell> string_output = dynamic_pointer_cast<StringCell>(col_v->get_cell(i));
                if (string_output) {
                    std::cout << string_output->get_value().second << " ";
                }
                std::shared_ptr<BytesCell> bytes_output = dynamic_pointer_cast<BytesCell>(col_v->get_cell(i));
                if (bytes_output) {
                    std::cout << "0x";
                    for (auto j : bytes_output->get_value().second) {
                        std::cout << static_cast<unsigned char>(j);
                    }
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl;
    }

    return 0;
}