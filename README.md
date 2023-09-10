# sqlite-wrapper-cpp

C++ Wrapper for SQLite

## Example

```cpp
#include <iostream>

int main() {
    try {
        // Create database, which will return error like an exception if something goes wrong
        sqlite::Database db("test.db", sqlite::ErrorType::EXCEPTION);

        // Create database, which will return error like a status code if something goes wrong
        // sqlite::Database db("test.db", sqlite::ErrorType::STATUS_CODE);

        // Setup database
        db.exec("DROP TABLE IF EXISTS Customers;");
        db.exec("CREATE TABLE Customers(customer_id int, first_name varchar(100), second_name varchar(100), age int);");

        sqlite::Result result;

        // Direct execution
        {
            sqlite::exec(db, "INSERT INTO Customers VALUES (1, 'John', 'Doe', 31);");
            sqlite::exec(db, "INSERT INTO Customers VALUES (2, 'Robert', 'Luna', 22);");
            sqlite::exec(db, "INSERT INTO Customers VALUES (3, 'David', 'Robinson', 22);");
        }

        // Accessing results by column name or number
        {
            result = sqlite::exec(db, "SELECT first_name, age FROM Customers WHERE first_name = 'John' AND age = 31;");

            // Accessing result by column name
            std::cout << result.get<string>("first_name") << " " << result.get<int>("age") << std::endl;

            // Accessing result by column number
            std::cout << result.get<string>(0) << " " << result.get<int>(1) << std::endl; // column number starts from 0.
        }

        // Accessing results row by row
        {
            result = sqlite::exec(db, "SELECT * FROM Customers;");
            for (size_t i = 0; i < result.rows.length; i++) {
                std::cout << result.get<string>("first_name") << " " << result.get<string>("second_name")<< " " << result.get<int>("age") << std::endl;
                result.next();
            }
        }

        // Binding parameters
        {
            sqlite::Statement statement(db);
            statement.set("INSERT INTO Customers (customer_id, first_name, second_name, age) VALUES (?, ?, ?, ?);");
            
            // Inserting values
            // First binding parameter index starts from 0.
            statement.bind(0, 4); // inserting customer_id
            statement.bind(1, "John"); // inserting first_name
            statement.bind(2, "Reinhardt"); // inserting second_name
            statement.bind(3, 29); // inserting age
            sqlite::exec(statement);

            // Inserting null values
            statement.set("INSERT INTO Customers (customer_id, first_name, second_name, age) VALUES (?, ?, ?, ?);");
            statement.bind(0, 5);
            statement.bind(1, "Thompson");
            statement.bind_null(2);
            statement.bind_null(3);
            sqlite::exec(statement);

            // Inserting multiple null values
            statement.set("INSERT INTO Customers (customer_id, first_name, second_name, age) VALUES (?, ?, ?, ?);");
            statement.bind_null(0, 3); // bind null from 0 to 3 parameters.
            sqlite::exec(statement);
        }

        // Transactions
        {
            {
                std::cout << "Deleting all rows from Customers table" << std::endl;
                sqlite::Transaction tx(db);
                sqlite::exec(db, "DELETE FROM Customers;");
                // transaction will be rolled back if we don't call tx.commit();
            }
            result = sqlite::exec("SELECT * FROM Customers;");
            std::cout << "still have " <<result.rows.length << " rows" << std::endl;
        }
    } catch (Exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }

    return 0;
}
```
