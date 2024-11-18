#ifndef QueryHEADER
#define QueryHEADER

#include <Table.h>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class Operation {
    public:
    virtual Table execute() = 0;
};

class OperationCreate: public Operation {
    // create table <name> (desc)
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
    public:
    OperationInsert(const std::string& args);
    Table execute() override;

    private:
    std::string table_name;
    std::vector<Column> values;
};

class OperationSelect: public Operation { // select <columns> from <table> where <condition>
    public:
    OperationSelect(const std::string& args);
    Table execute() override;

    private:
    std::vector<std::string> column_names;
    Table table;
    // some type Condition (may be lambda)    
};

class OperationSelect: public Operation { // select <columns> from <table> where <condition>
    public:
    OperationSelect(const std::string& args);
    Table execute() override;

    private:
    std::vector<std::string> column_names;
    Table table;
    // Condition (may be lambda)    
};

class OperationUpdate : public Operation {
    public:
    OperationUpdate(const std::string& args);
    Table execute() override;

    private:
    Table table;
    // Condition
    // Assignments
};

class OperationDelete : public Operation {
    public:
    OperationDelete(const std::string& args);
    Table execute() override;

    private:
    Table table; // single table
    // Condition
};

class OperationJoin : public Operation {
    public:
    OperationJoin(const std::string& args);
    Table execute() override;

    private:
    Table table1;
    Table table2;
    // Condition
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
};

#endif // QueryHEADER