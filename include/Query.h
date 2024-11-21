#ifndef QueryHEADER
#define QueryHEADER

#include <Table.h>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

enum OP_TYPE {
    CREATE_TABLE = 0,
    INSERT,
    SELECT,
    UPDATE,
    DELETE,
    JOIN,
    CREATE_U_INDEX,
    CREATE_O_INDEX
};

class Operation {
    public:
    virtual Table execute() = 0;
};

class OperationCreate: public Operation {
    // create table <name> (col1, col2)
    // col looks like:
    // [{<a1>, <a2>,...}] <name>: <type> [= value]
    // [] holds unnecessary params
    // [] are also abscent in actual query
    public:
    OperationCreate(const std::string& args);
    Table execute() override;

    private:
    std::string table_name;
    std::vector<std::vector<std::string>> col_attributes;
    std::vector<std::string> col_name;
    std::vector<std::string> col_type;
    std::vector<int> col_sizeof;
    std::vector<std::string> col_default_value;
};

class OperationInsert: public Operation {
    // insert (<values>) to <table>
    // values are pairs like <name> = <value>
    public:
    OperationInsert(const std::string& args);
    Table execute() override;

    private:
    std::string table_name;
    std::vector<std::vector<std::string>> col_attributes;
    std::vector<std::string> col_name;
    std::vector<std::string> col_type;
    std::vector<int> col_sizeof;
    std::vector<std::string> col_default_value;
};

class OperationSelect: public Operation {
    // select <columns> from <table> where <condition>
    public:
    OperationSelect(const std::string& args);
    Table execute() override;

    private:
    std::vector<std::string> column_names;
    std::string table_name;
    // some type Condition (may be lambda)    
};

class OperationUpdate : public Operation {
    // update <table> set <assignments> where <condition>
    public:
    OperationUpdate(const std::string& args);
    Table execute() override;

    private:
    std::string table_name;
    // Condition
    // Assignments
};

class OperationDelete : public Operation {
    // delete <table> where <condition>
    public:
    OperationDelete(const std::string& args);
    Table execute() override;

    private:
    std::string table_name; // single table
    // Condition
};

class OperationJoin : public Operation {
    // <table1> join <table2> on <condition>
    public:
    OperationJoin(const std::string& args);
    Table execute() override;

    private:
    std::string first_table_name;
    std::string second_table_name;
    // Condition
};

class OperationOrderedIndex : public Operation {
    // create ordered index on <table> by <columns>
    public:
    OperationOrderedIndex(const std::string& args);
    Table execute() override;
    
};

class OperationUnorderedIndex : public Operation {
    // create unordered index on <table> by <columns>
    public:
    OperationUnorderedIndex(const std::string& args);
    Table execute() override;
    
};

class Comb: public Operation {
    public:
    Comb(const Operation& left, const Operation& right);
    Table execute() override;

    private:
    std::shared_ptr<Operation> left;
    std::shared_ptr<Operation> right;
};

class Query {
    public:
    Query(const std::string& query);
    std::shared_ptr<Operation> compile();

    private:
    std::vector<std::shared_ptr<Operation>> op;
    std::vector<std::pair<std::vector<std::vector<std::string>>, char>> op_str;
};

#endif // QueryHEADER