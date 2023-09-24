#ifndef __SQLITE_HPP__
#define __SQLITE_HPP__

#include <string>
#include <vector>

namespace sqlite {
    class Database {
    public:
        Database(const std::string& path);
    };

    class Error {
    public:
        Error(const int code, const std::string& msg);
        const int code() const;
        const std::string& msg() const;
    };

    class Statement {
    public:
        Statement(Database& db, const std::string& sql);

        template <typename T>
        const Error& bind(const size_t index, const T& value);
    };

    class Transaction {
    public:
        Transaction(Database& db);
        const Error& commit();
    };

    class Results {
    private:
        class ResultIterator {
        public:
            const ResultIterator& operator++();

            template <typename T>
            const T& get(const std::string& column_name) const;

            template <typename T>
            const T& get(const std::string& column_index) const;
        };
    public:
        typedef ResultIterator iterator;

        size_t count() const;

        template <typename T>
        const T& get(const std::string& column_name) const;

        template <typename T>
        const T& get(const std::string& column_index) const;

        const iterator& begin() const;
        const iterator& end() const;
    };

    const Error exec(Database& db, const std::string& sql);
};

#endif // __SQLITE_HPP__
