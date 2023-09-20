# sqlite-wrapper-cpp

C++ Wrapper for SQLite

## Example

```cpp
#include <iostream>

sqlite::Error setup(const sqlite::Database &db) {
    sqlite::Error err = sqlite::exec(db, "DROP TABLE IF EXISTS Customers;");
    if (err) {
        return err;
    }

    err = sqlite::exec(db, "CREATE TABLE Customers(customer_id int, first_name varchar(100), second_name varchar(100), age int);");
    if (err) {
        return err;
    }

    return sqlite::OK;
}

int main() {
    // Create database, which will return error like a status code if something goes wrong
    sqlite::Error err = sqlite::Database db("test.db", sqlite::ErrorTypes.STATUS_CODE);
    if (err) { 
        std::cerr << err.msg() << std::endl;
        return err; 
    }

    // try {
    //     // Create database, which will return error like an exception if something goes wrong
    //     sqlite::Database db("test.db", sqlite::ErrorTypes.EXCEPTION);
    // } catch (std::Exception &e) {
    //     std::cout << "exception: " << e.what() << std::endl;;
    // }

    err = setup(db);
    if (err) { 
        std::cerr << err.msg() << std::endl;
        return err; 
    }

    sqlite::Results results;

    // Direct execution
    {
        if (err = sqlite::exec(db, "INSERT INTO Customers VALUES (1, 'John', 'Doe', 31);"); err) { return err; }
        if (err = sqlite::exec(db, "INSERT INTO Customers VALUES (2, 'Robert', 'Luna', 22);"); err ) { return err; }
        if (err = sqlite::exec(db, "INSERT INTO Customers VALUES (3, 'David', 'Robinson', 22);"); err) { return err; }
    }

    // Accessing results by column name or number
    {
        if (err = sqlite::exec(db, "SELECT first_name, age FROM Customers WHERE first_name = 'John' AND age = 31;", results); err) { return err; }

        if (results.count()) {
            // Accessing result by column name
            std::cout << result.get<string>("first_name") << " " << result.get<int>("age") << std::endl;

            // Accessing result by column number
            std::cout << result.get<string>(0) << " " << result.get<int>(1) << std::endl; // column number starts from 0.
        }

        // Accessing result by not existent column name will be returned as null
        std::cout << result.get<string>("not_existent") << std::endl;
    }

    // Accessing results row by row
    {
        
        if (err = sqlite::exec(db, "SELECT * FROM Customers;", results); err) { return err; }

        for (sqlite::Results_iterator it = results.begin(); it != results.end(); it++) {
            std::cout << it->get<std::std::string>("first_name") << " " << it->get<int>("age") << std::endl;
        }
    }

    // Binding parameters
    {
        // Inserting values
        {
            sqlite::Statement statement(db, "INSERT INTO Customers (customer_id, first_name, second_name, age) VALUES (?, ?, ?, ?);");
            
            // First binding parameter index starts from 0.
            statement.bind(0, 4); // inserting customer_id
            statement.bind(1, "John"); // inserting first_name
            statement.bind(2, "Reinhardt"); // inserting second_name
            statement.bind(3, 29); // inserting age
            if (err = sqlite::exec(statement); err) { return err; };
        }

        // Inserting null values
        {
            // Missed values will be automatically bind with null
            sqlite::Statement statement(db, "INSERT INTO Customers (customer_id, first_name, second_name, age) VALUES (?, ?, ?, ?);");
            statement.bind(0, 5);
            statement.bind(1, "Thompson");
            if (err = sqlite::exec(statement); err) { return err; };
        }
        
        // Inserting multiple null values
        {
            // Missed values will be automatically bind with null
            sqlite::Statement statement(db, "INSERT INTO Customers (customer_id, first_name, second_name, age) VALUES (?, ?, ?, ?);");
            if (err = sqlite::exec(statement); err) { return err; };
        }
    }

    // Transactions
    {
        {
            std::cout << "Deleting all rows from Customers table" << std::endl;
            sqlite::Transaction tx(db);
            if (err = sqlite::exec(db, "DELETE FROM Customers;"); err) { return err; };
            // transaction will be rolled back if we don't call tx.commit();
        }
        if (err = sqlite::exec(db, "SELECT * FROM Customers;", results); err) { return err; };
        std::cout << "still have " << results.end() - results.begin() << " rows" << std::endl;
    }

    return 0;
}
```
