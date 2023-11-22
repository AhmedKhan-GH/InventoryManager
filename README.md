# InventoryManager

```
SQLite database with C++ logic and wxWidgets interface for management of non-salable permanent possessions in personal and business environments
```

- Achievements:
	- Data Access Layer
		- universal table creation query for any schema
		- modular prepared statements with overloaded binders for various data types
		- extensible for future DAO's from a derived from a virtual interface generic DAO class

- Next Steps:
	- Generic DAO to begin creating schema/table specific classes
	- each dao will have unique prepared statements and data queries based on project requirements
	- need to run and document tests with main()
