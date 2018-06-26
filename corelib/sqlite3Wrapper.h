#ifndef _________________sqlite3Wrapper__h
#define _________________sqlite3Wrapper__h
#include "_QUERY.h"
struct sqlite3;

/// C++ simple wrapper for sqlite3

class Sqlite3Wrapper
{
public:
    Sqlite3Wrapper(const std::string& name);
    virtual ~Sqlite3Wrapper();
    bool inTransaction;
    bool noErrors;

    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
    void execSimple(const QUERY &) const;
    void execSimple(const std::string &) const;
    REF_getter<QueryResult> exec(const QUERY &)const;
    REF_getter<QueryResult> exec(const std::string &query)const;




    std::string select_1(const QUERY &) const;

    std::vector<std::string> select_1_row(const QUERY&)const;

    std::vector<std::string> select_1_column(const QUERY&)const;


    std::string select_1(const std::string &)const;

    std::vector<std::string> select_1_row(const std::string &)const;

    std::vector<std::string> select_1_column(const std::string &)const;


private:
    sqlite3 *dbh;
    std::string dbname;

protected:
};

#endif
