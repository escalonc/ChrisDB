// ChrisDB.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "engine.h"

int main()
{
    std::cout << "Creating database!\n";
	char database_name[] = "chris.db";
    const auto database_engine = new engine(database_name);
	database_engine->create_database(100, 512);
	auto table_info = new table();

	strcpy_s(table_info->name,30, "example_table");
	table_info->first_column = 5;
	database_engine->create_table(table_info);
	char table_name[30] = "example_table";
	auto read_table = database_engine->find_table_by_name(table_name);
		
}