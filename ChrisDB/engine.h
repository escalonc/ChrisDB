#pragma once
#include "data_file.h"
#include "types.h"
#include <tuple>

class engine
{
	data_file* data_file_;
	database_header* database_header_;
	
public:
	explicit engine(char* name);
	void create_database(int database_size, int data_block_size) const;
	void create_table(table* table_info, column* columns_info, unsigned int columns_quantity) const;
	table* find_table_by_name(char name[30]) const;
	column* find_columns_of_table(table* table_info);
	std::tuple<int, int> find_available_data_block(char block_type) const;
	std::tuple<unsigned int, unsigned int> create_columns(column* columns_info, unsigned int columns_quantity) const;
};
