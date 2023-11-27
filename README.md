# InventoryManager

```
SQLite database with C++ logic and wxWidgets interface for management of non-salable permanent possessions in personal and business environments
```

- Achievements:
	- Data Access Layer (DatabaseManager.hpp)
		- universal table creation query for any schema
		- modular prepared statements with binders for various data types, with type safety ensuring sqlite3 dynamic typing does not end up with wrong data types within the table
		- extensible for future DAO's from a derived from a virtual interface generic DAO class, building on database manager according to solid principles
		- password hashing and validation class using SHA1 as proof of concept

- Next Steps:
	- Generic DAO to begin creating schema/table specific classes
	- each dao will have unique prepared statements and data queries based on project requirements
