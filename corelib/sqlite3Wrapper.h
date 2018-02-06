#ifndef _________________sqlite3Wrapper__h
#define _________________sqlite3Wrapper__h
#include "_QUERY.h"
struct sqlite3;
class Sqlite3Wrapper
{
public:
    Sqlite3Wrapper(const std::string& name);
    virtual ~Sqlite3Wrapper();


    void execSimple(const QUERY &) const;
    void execSimple(const std::string &) const;
    REF_getter<QueryResult> exec(const QUERY &)const;
    REF_getter<QueryResult> exec(const std::string &query)const;



    //void makeConnection(const sqliteConf& conf);

    std::string select_1(const QUERY &) const;

    /// получить 1 строку. Примечание строка должна действительно выбраться.
    std::vector<std::string> select_1_row(const QUERY&)const;

    /// получить 1 столбец. Примечание столбец должен действительно выбраться.
    std::vector<std::string> select_1_column(const QUERY&)const;


    /// получить 1 элемент из запроса типа "select last_insert_rowid()". Примечание element должeн действительно выбраться.
    std::string select_1(const std::string &)const;

    /// получить 1 строку. Примечание строка должна действительно выбраться.
    std::vector<std::string> select_1_row(const std::string &)const;

    /// получить 1 столбец. Примечание столбец должен действительно выбраться.
    std::vector<std::string> select_1_column(const std::string &)const;


private:
    sqlite3 *dbh;
    std::string dbname;

protected:
};

#endif
