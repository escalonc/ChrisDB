#pragma once
#include "data_file.h"
#include "types.h"

class engine
{
	data_file* data_file_;
	database_header* database_header_;
public:
	explicit engine(char* name);
	void create_database(int database_size, int data_block_size) const;
	
};
