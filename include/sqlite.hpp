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
    private:
        static const size_t DEFAULT_MSG_LENGTH = 100;
        void copy(const Error &other) {
            code = other.code;
            if (other.msg) {
                msg = new char[DEFAULT_MSG_LENGTH];
                strncpy(msg, other.msg, DEFAULT_MSG_LENGTH);
            }
            else {
                msg = other.msg;
            }
        }

    public:
        int code;
        char *msg;

        Error(const int _code, char *_msg) : code(_code) {
            if (_msg) {
                msg = new char[strlen(_msg) + 1];
                strncpy(msg, _msg, strlen(_msg) + 1);
            }
            else {
                msg = nullptr;
            }
        }
        Error() : code(SQLITE_OK), msg(new char[DEFAULT_MSG_LENGTH]) {}
        Error(const Error &other) {
            copy(other);
        };

        Error &operator=(const Error &other) {
            if (this != &other) {
                copy(other);
            }

            return *this;
        }
        operator bool() const {
            return code == SQLITE_OK;
        }

        ~Error() {
            if (msg) {
                delete[] msg;
            }
        }
    };

    const Error OK(SQLITE_OK, nullptr);

    class Database;
    class Statement;

    class Results {
    private:
        class ResultIterator {
        private:
            std::map<std::string, std::string> m_raw;
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
        friend Error exec(Database &db, const Statement &st, Results &res);
    };

    class Database {
    private:
        sqlite3 *m_db;
    public:
        Database(const std::string &path, const ErrorTypes &type = ErrorTypes::STATUS_CODE) {
            if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK) {
                std::cerr << "Cannot create/open database: " << path << std::endl;
            }
        }

        Database(const Database &other) {
        }

        ~Database() {
            sqlite3_close(m_db);
        }

        friend Error exec(Database &db, const char *sql);
        friend Error exec(Database &db, const Statement &st);
        friend Error exec(Database &db, const char *sql, Results &res);
        friend Error exec(Database &db, const Statement &st, Results &res);
    };

    class Statement {
        std::string m_sql;
        int m_bind_param_count;
        static const char BIND_SYMBOL = '?';
    public:
        Statement(const char *sql)
            : m_sql(sql), m_bind_param_count(std::count(m_sql.begin(), m_sql.end(), BIND_SYMBOL)) {}

        Error bind(const size_t index, const std::string &value) {
            if (index < m_bind_param_count) {
                for (size_t i = 0, pos = 0; pos != std::string::npos; pos++, i++) {
                    pos = m_sql.find(BIND_SYMBOL, pos);
                    if (i == index) {
                        m_sql.insert(pos, value);
                        return OK;
                    }
                }
            }

            return Error(SQLITE_NOTFOUND, (char *)("Invalid bind parameter index"));
        }

        friend Error exec(Database &db, const Statement &st);
        friend Error exec(Database &db, const Statement &st, Results &res);
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
            result.m_raw[columns[i]] = data[i];
        }

        results->m_results.push_back(result);

        return 0;
    }

    Error exec(Database &db, const char *sql) {
        Error err;
        err.code = sqlite3_exec(db.m_db, sql, nullptr, nullptr, &err.msg);
        if (err.code != SQLITE_OK) {
            return err;
        }

        return OK;
    }

    Error exec(Database &db, const char *sql, Results &res) {
        res.m_results.clear();

        Error err;
        err.code = sqlite3_exec(db.m_db, sql, my_special_callback, &res, &err.msg);
        if (err.code != SQLITE_OK) {
            return err;
        }

        return OK;
    }

    Error exec(Database &db, const Statement &st) {
        Error err;
        err.code = sqlite3_exec(db.m_db, st.m_sql.c_str(), nullptr, nullptr, &err.msg);
        if (err.code != SQLITE_OK) {
            return err;
        }

        return OK;
    }

    Error exec(Database &db, const Statement &st, Results &res) {
        res.m_results.clear();

        Error err;
        err.code = sqlite3_exec(db.m_db, st.m_sql.c_str(), my_special_callback, &res, &err.msg);
        if (err.code != SQLITE_OK) {
            return err;
        }

        return OK;
    }
};

#endif // __SQLITE_HPP__
