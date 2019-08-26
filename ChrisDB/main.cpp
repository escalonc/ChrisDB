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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
