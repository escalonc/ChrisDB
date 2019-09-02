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

	const auto columns = new column();

	char table_1[30] = "example_table_1";
	char table_2[30] = "example_table_2";
	char table_3[30] = "example_table_3";
	char table_4[30] = "example_table_4";
	
	database_engine->create_table(table_1, columns, 1);
	database_engine->create_table(table_2, columns, 1);
	database_engine->create_table(table_3, columns, 1);
	database_engine->create_table(table_4, columns, 1);
	
	char table_name[30] = "example_table_2";
	auto read_table = database_engine->find_table_by_name(table_name);
	std::cout << "Database was created" << std::endl;
	delete database_engine;
}