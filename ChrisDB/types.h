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
	int data_blocks_quantity;
	int data_block_size;
	int first_block;
	char database_name[30];
};

struct data_block
{
	char* data;
	int next;
};
