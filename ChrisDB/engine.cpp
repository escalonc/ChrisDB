#include "engine.h"
#include "types.h"

engine::engine(char* name)
{
	this->data_file_ = new data_file(name);
	this->database_header_ = new database_header();
	// strcat_s(this->database_name_, strlen(name), name);
}

void engine::create_database(const int database_size, const int data_block_size) const
{
	this->data_file_->open(std::ios::in | std::ios::out | std::ios::app | std::ios::binary);

	const auto quantity_from_mb_to_b = 1024 * 1024 * database_size;
	this->database_header_->data_block_size = data_block_size;
	this->database_header_->data_blocks_quantity = (quantity_from_mb_to_b - sizeof(database_header)) / data_block_size;
	this->database_header_->data_block_buffer_size = data_block_size - sizeof data_block;
	this->database_header_->first_data_block = sizeof(database_header);
	database_header_->bitmap = std::vector<bool>(database_header_->data_blocks_quantity);
	// strcat_s(this->database_header_->database_name, strlen(database_header_->database_name), this->database_name_);


	const auto block = new data_block();
	block->data = new char[this->database_header_->data_block_buffer_size];

	const auto quantity = this->database_header_->data_blocks_quantity;

	this->data_file_->write(reinterpret_cast<char*>(this->database_header_), sizeof(database_header_));

	for (unsigned int i = 0; i < quantity; i++)
	{
		this->data_file_->write(reinterpret_cast<char*>(block), database_header_->data_block_size);
	}

	this->data_file_->close();
	this->data_file_->open(std::ios::in | std::ios::out | std::ios::binary);
}

void engine::create_table(table* table_info) const
{
	auto position = database_header_->first_data_block;

	for (unsigned int i = 0; i < database_header_->bitmap.size(); i++)
	{
		if (!database_header_->bitmap[i])
		{
			position += database_header_->data_block_size;
			return;
		}
	}

	const auto table_block = reinterpret_cast<data_block*>(data_file_->read(position, database_header_->data_block_size));
	table_block->data = new char[database_header_->data_block_buffer_size];
	table_block->object_type = 'T';
	table_block->objects_amount++;
	const auto first_byte = table_block->first_free_byte;
	memcpy(&table_block->data[first_byte], reinterpret_cast<char*>(table_info), sizeof table);
	table_block->first_free_byte += sizeof table;

	this->data_file_->write(reinterpret_cast<char*>(table_block), this->database_header_->first_data_block, database_header_->first_data_block);
}

table* engine::find_table_by_name(char name[30]) const
{
	auto block_position = database_header_->first_data_block;

	for (unsigned int i = 0; i < database_header_->data_blocks_quantity; i++)
	{
		const auto current_data_block = reinterpret_cast<data_block*>(data_file_->read(block_position,
			database_header_->data_block_size));

		if (current_data_block->object_type != 'T')
		{
			continue;
		}

		const auto buffer = new char[sizeof table];

		for (unsigned int j = 0; j < current_data_block->objects_amount;j++)
		{
			memcpy(buffer, &current_data_block->data[j], sizeof table);
			const auto current_table = reinterpret_cast<table*>(buffer);

			if (strcmp(current_table->name, name) == 0)
			{
				return current_table;
			}
		}

		block_position += database_header_->data_block_size;
	}

	return nullptr;
}
