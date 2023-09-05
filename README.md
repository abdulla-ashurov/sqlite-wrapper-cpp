# sqlite-wrapper-cpp

C++ Wrapper for SQLite

## Classes

### Database

Allow open/create database file.

#### Methods of Database class

- `Database()` - default constructor.
- `Database(const std::string &filename, OpenMode openMode = OpenMode::READWRITE)` - open/create a database file based on filename and openMode parameters. If only the first parameter is passed to the constructor then the database will be opened in READ|WRITE mode. Available combine multiple open modes.
- `int open(const std::string &filename, OpenMode openMode = OpenMode::READWRITE)` - open/create a database file based on filename and openMode parameters. If only the first parameter is passed to the constructor then the database will be opened in READ|WRITE mode. Available combine multiple open modes.
- `int close()` - close a database file and return a status code.
- `execute(const std::string &sql_code)` - execute a SQL statement.

### Statement

Allow to execute a SQL statements.

#### Methods of Statement class

- `Statement(const Database &db, const std::string &sql_code)` - create a SQL statement.
- `void bind(const size_t argc, const std::string &argv...)` - change all `?` symbols in SQL statement based on `argc` (argument count) and `argv` (argument values).
- `execute()` - execute a SQL statement.

### Transaction

Allow to run SQLite transactions.

#### Methods of Transaction class

- `Transaction(const Database &db)` - create a transaction.
- `int commit()` - commit transaction.
