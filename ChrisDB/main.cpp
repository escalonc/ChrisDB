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
	char table_name_1[30] = "ID";
	char table_name_2[30] = "NAME";

	const auto column_info = new column_dto*[2]{ new column_dto(), new column_dto() };
	column_info[0]->data_type = 'I';
	strcpy_s(column_info[0]->name, 30, table_name_1);
	column_info[0]->size = 4;

	column_info[1]->data_type = 'C';
	strcpy_s(column_info[1]->name, 30, table_name_2);
	column_info[1]->size = 20;

	char table_1[30] = "example_table_1";
	
	database_engine->create_table(table_1, column_info, 2);
	
	const auto read_table = database_engine->find_table_by_name(table_1);

	const auto table_columns = database_engine->find_columns_of_table(read_table);

	std::cout << table_columns[0]->name << std::endl;
	std::cout << table_columns[0]->size << std::endl << std::endl;

	std::cout << table_columns[1]->name << std::endl;
	std::cout << table_columns[1]->size << std::endl << std::endl;
	
	
	std::cout << "Database was created" << std::endl;
	delete database_engine;
}