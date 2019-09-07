#pragma once
#include <vector>


struct table
{
	char name[30]{};
	unsigned int columns_amount{};
	int first_record{};
	int first_block_column_byte_location = -1;
	int first_column_byte_location_in_block = -1;
	int first_block_record_byte_location = -1;
	int first_record_byte_location_in_block = -1;
};

struct column
{
	unsigned int size{};
	int next_block_column_byte_location = -1;
	int next_column_byte_location_in_block = -1;
	char name[30]{};
	char data_type{};
};

struct column_dto
{
	char name[30]{};
	char data_type{};
	unsigned int size{};
};

struct record
{
	char* data;
	int next;
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
	unsigned int remaining_space{};
	int next_data_block_byte_location = -1;
	char object_type = 'E';
	char* data{};
};
