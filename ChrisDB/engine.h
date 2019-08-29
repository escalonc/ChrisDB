#pragma once
#include "data_file.h"
#include "types.h"

class engine
{
	data_file* data_file_;
	database_header* database_header_;
	char* database_name_;
public:
	explicit engine(char* name);
	void create_database(int database_size, int data_block_size) const;
	void create_table(table* table_info) const;
	table* find_table_by_name(char name[30]) const;
	int find_available_data_block(char block_type) const;
	void create_columns(column* columns_info, unsigned int columns_quantity) const;
	
};
