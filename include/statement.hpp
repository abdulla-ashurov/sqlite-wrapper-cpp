#ifndef __STATEMENT_HPP__
#define __STATEMENT_HPP__

#include "database.hpp"

class Statement {
public:
    Statement(const Database &db, const std::string &sql_code);
    
    void bind(const size_t argc, const std::string &argv...);
    int execute();
};

#endif // __STATEMENT_HPP__
