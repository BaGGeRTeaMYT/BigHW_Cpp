#include <iostream>
#include <Query.h>

int main() {
    // should begin with create table
    OperationCreate tst = OperationCreate(std::string("table_name ({attr1, attr2,attr3}  col1 : int32 = 31, {attr2,attr3}  col2: bytes[12] = 0x1234 )"));
    // new tests required
    return 0;
}