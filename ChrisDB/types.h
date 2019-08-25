#pragma once

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
	unsigned int data_block_size;
	unsigned int net_data_block_size;
	int first_data_block;
	char database_name[30];
};

struct data_block
{
	unsigned int first_free_byte = 0;
	char object_type;
	char* data;
	int next;
};
