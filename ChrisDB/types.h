#pragma once
#include <vector>


struct table
{
	char name[30];
	unsigned int first_block_column_byte_location;
	unsigned int first_column_byte_location_in_block;
};

struct column
{
	char name[30];
	char data_type[30];
	unsigned int next_block_column_byte_location;
	unsigned int next_column_byte_location_in_block;
};

struct database_header
{
	unsigned int data_blocks_quantity{};
	unsigned int data_block_buffer_size{};
	unsigned int data_block_size{};
	unsigned int first_data_block{};
	int first_table_block_byte_location = -1;
	char database_name[30]{};
	std::vector<bool> bitmap;
};

struct data_block
{
	unsigned int first_free_byte = 0;
	unsigned int objects_amount = 0;
	unsigned int remaining_space {};
	int next_data_block_byte_location = -1;
	char object_type = 'E';
	char* data{};
};
