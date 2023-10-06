#ifndef __SQLITE_HPP__
#define __SQLITE_HPP__

#include <string>
#include <vector>
#include <sqlite3.h>

namespace sqlite {
    enum ErrorTypes {
        EXCEPTION = 0,
        STATUS_CODE = 1
    };

    class Error {
    public:
        Error(const int code, const char *msg) {}
        const int code() const;
        const std::string &msg() const;
        operator bool() const {

        }
    };

    const Error OK(0, "");

    class Database;

    class Results {
    private:
        class ResultIterator {
        private:
            std::map<std::string, std::string> raw;
        public:
            const ResultIterator &operator++();

            template <typename T>
            const T &get(const std::string &column_name) const;

            template <typename T>
            const T &get(const int &column_index) const;

            friend int my_special_callback(void *res, int count, char **data, char **columns);
        };

        std::vector<ResultIterator> m_results;
    public:
        typedef ResultIterator iterator;

        size_t count() const {
            return m_results.size();
        }

        template <typename T>
        const T &get(const std::string &column_name) const;

        template <typename T>
        const T &get(const int &column_index) const;

        const iterator &begin() const;
        const iterator &end() const;

        friend int my_special_callback(void *res, int count, char **data, char **columns);
        friend Error exec(Database &db, const char *sql, Results &res);
    };

    class Database {
    private:
        sqlite3 *db;
    public:
        Database(const std::string &path, const ErrorTypes &type = ErrorTypes::STATUS_CODE) {
            if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
                std::cerr << "Cannot create/open database: " << path << std::endl;
            }
        }

        friend Error exec(Database &db, const char *sql, Results &res);
        friend Error exec(Database &db, const char *sql);
    };

    class Statement {
    public:
        Statement(Database &db, const std::string &sql);

        template <typename T>
        const Error &bind(const size_t index, const T &value);
    };

    class Transaction {
    public:
        Transaction(Database &db);
        const Error &commit();
    };

    /*
    * SQLite will execute the passed SQL statement and for every result row
    * that it finds it will call my_special_callback.
    */
    int my_special_callback(void *res, int count, char **data, char **columns) {
        Results *results = (Results *)(res);

        Results::iterator result;
        for (size_t i = 0; i < count; i++) {
            result.raw[columns[i]] = data[i];
        }

        results->m_results.push_back(result);

        return 0;
    }

    Error exec(Database &db, const char *sql) {
        char **err_msg = new char *[1];
        err_msg[0] = new char[100];
        int status_code = sqlite3_exec(db.db, sql, my_special_callback, nullptr, err_msg);
        if (status_code != SQLITE_OK) {
            return Error(status_code, *err_msg);
        }

        delete err_msg[0];
        delete[] err_msg;

        return OK;
    }

    Error exec(Database &db, const char *sql, Results &res) {
        res.m_results.clear();
        char **err_msg = new char *[1];
        err_msg[0] = new char[100];
        int status_code = sqlite3_exec(db.db, sql, my_special_callback, &res, err_msg);
        if (status_code != SQLITE_OK) {
            return Error(status_code, *err_msg);
        }

        delete err_msg[0];
        delete[] err_msg;

        return OK;
    }
};

#endif // __SQLITE_HPP__
