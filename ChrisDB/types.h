#pragma once
#include <vector>


struct table
{
	char name[30];
	int first_column;
};

struct column
{
	char name[30];
	char data_type[30];
	int next;
};

struct database_header
{
	unsigned int data_blocks_quantity;
	unsigned int data_block_buffer_size;
	unsigned int data_block_size;
	int first_data_block;
	char database_name[30];
	std::vector<bool> bitmap;
};

struct data_block
{
	unsigned int first_free_byte = 0;
	unsigned int objects_amount = 0;
	int next;
	char* data;
	char object_type;
};
