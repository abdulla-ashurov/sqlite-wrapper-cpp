#ifndef __TRANSACTION_HPP__
#define __TRANSACTION_HPP__

#include "database.hpp"

class Transaction {
public:
    explicit Transaction(const Database &db);

    int commit();
};

#endif // __TRANSACTION_HPP__
