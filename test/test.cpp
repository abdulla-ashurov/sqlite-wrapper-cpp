#include <iostream>
#include "catch_amalgamated.hpp"
#include "../include/sqlite.hpp"

class TestSetup {
public:
    sqlite::Database db;

    TestSetup(): db("test.db") {
        if(sqlite::exec(db, "DROP TABLE IF EXISTS Customers;")) {
            std::cerr << "Drop Customers table" << std::endl;
        }

        if (sqlite::exec(db, "CREATE TABLE Customers(id int, name varchar(100), age int);")) {
            std::cerr << "Cannot create Customers table" << std::endl;
        }
    }
};

TEST_CASE("Test Statement class") {
    SECTION("It can bind parameters by name in a statement") {
        TestSetup setup;
        const int expected_id = 1, expected_age = 29;
        const std::string expected_name = "John";
            
        sqlite::Statement statement(setup.db, "INSERT INTO Customers (id, name, age) VALUES (?, ?, ?);");
        statement.bind(0, expected_id); // inserting id
        statement.bind(1, expected_name); // inserting name
        statement.bind(2, expected_age); // inserting age
        sqlite::exec(statement);

        sqlite::Results results;
        sqlite::exec(setup.db, "SELECT name, age FROM Customers WHERE name = 'John' AND age = 29;", results);

        REQUIRE(results.count() == 1);
        REQUIRE(results.get<int>("id") == expected_id);
        REQUIRE(results.get<std::string>("name") == expected_name);
        REQUIRE(results.get<int>("age") == expected_age);
    }

    SECTION("It can automatically bind with null missed parameters") {
        TestSetup setup;
        const int expected_id = 1;
        const std::string expected_name = "John";
            
        sqlite::Statement statement(setup.db, "INSERT INTO Customers (id, name, age) VALUES (?, ?, ?);");
        statement.bind(0, expected_id); // inserting id
        statement.bind(1, expected_name); // inserting name
        // Missed age parameter
        sqlite::exec(statement);

        sqlite::Results results;
        sqlite::exec(setup.db, "SELECT name, age FROM Customers WHERE name = 'John' AND age = 29;", results);

        REQUIRE(results.count() == 1);
        REQUIRE(results.get<int>("id") == expected_id);
        REQUIRE(results.get<std::string>("name") == expected_name);
        REQUIRE(results.get<int>("age") == NULL);
    }

    SECTION("It can automatically bind with null multiple missed parameters") {
        TestSetup setup;
            
        sqlite::Statement statement(setup.db, "INSERT INTO Customers (id, name, age) VALUES (?, ?, ?);");
        // Missed id, name, age parameters
        sqlite::exec(statement);

        sqlite::Results results;
        sqlite::exec(setup.db, "SELECT name, age FROM Customers WHERE name = 'John' AND age = 29;", results);

        REQUIRE(results.count() == 1);
        REQUIRE(results.get<int>("id") == NULL);
        REQUIRE(results.get<std::string>("name") == std::string(""));
        REQUIRE(results.get<int>("age") == NULL);
    }

    SECTION("It cannot bind parameter with an invalid column id") {
        TestSetup setup;
        const int invalid_column_id = 10, value = 1;
        const std::string expected_err_msg = "An invalid column id";
            
        sqlite::Statement statement(setup.db, "INSERT INTO Customers (id, name, age) VALUES (?, ?, ?);");
        statement.bind(invalid_column_id, value);
        sqlite::Error err = sqlite::exec(statement);
        REQUIRE(err.msg() == expected_err_msg);
    }
}

TEST_CASE("Test Transaction class") {
    SECTION("It can commit a transaction") {
        TestSetup setup;
        const std::string expected_err_msg = "Customers table does not exist";

        sqlite::Transaction tx(setup.db);
        sqlite::exec(setup.db, "DELETE FROM Customers;");
        tx.commit();

        sqlite::Error err = sqlite::exec(setup.db, "SELECT * FROM Customers;");
        REQUIRE(err.msg() == expected_err_msg);
    }

    SECTION("It can rollback a transaction") {
        TestSetup setup;
        sqlite::exec(setup.db, "INSERT INTO Customers (id, name, age) VALUES (1, 'John', 18);");

        {
            sqlite::Transaction tx(setup.db);
            sqlite::exec(setup.db, "DELETE FROM Customers;");
           // transaction will be rolled back if we don't call tx.commit();
        }

        sqlite::Results results;
        sqlite::Error err = sqlite::exec(setup.db, "SELECT * FROM Customers;", results);
        REQUIRE(results.count() == 1);
    }
}
