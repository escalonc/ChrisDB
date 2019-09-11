#pragma once
#include "data_file.h"
#include "types.h"
#include <tuple>

class engine
{
	data_file* data_file_;
	database_header* database_header_;
	static column* column_factory(column_dto* column_info);
	std::tuple<int, int> find_available_data_block(char block_type) const;
	std::tuple<int, int> find_available_data_block(char block_type, unsigned exclude_position) const;

public:
	explicit engine(char* name);
	void create_database(unsigned database_size, unsigned data_block_size) const;
	void create_table(char* table_name, column_dto** columns_info, unsigned int columns_amount) const;
	std::tuple<table*, unsigned, unsigned> find_table_by_name(char name[30]) const;
	column** find_columns_of_table(table* table_info) const;
	std::tuple<unsigned int, unsigned> create_columns(column_dto** columns_info, unsigned int columns_amount) const;
	unsigned get_record_size(table* table_info) const;
	void create_record(char* input_buffer, table* table_info, unsigned block_table_byte_location, unsigned table_byte_location_in_block) const;
	void close() const;
	void select_all(table* table_info) const;
	void list_tables() const;
	void delete_table(char name[30]) const;
	void delete_records(char table_name[30]) const;
};
