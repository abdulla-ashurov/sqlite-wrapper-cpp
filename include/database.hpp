#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <string>

enum OpenMode {
    READONLY = 0b00000001,
    READWRITE = 0b00000010,
    CREATE = 0b00000011
};

class Database {
public:
    Database() = default;
    Database(const std::string &filename, OpenMode openMode = OpenMode::READWRITE);

    int open(const std::string &filename, OpenMode openMode = OpenMode::READWRITE);
    int close();

    int execute(const std::string &sql_code);

    ~Database();
};

#endif // __DATABASE_HPP__
