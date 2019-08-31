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
	block->object_type = 'E';
	block->data = new char[this->database_header_->data_block_buffer_size];

	const auto quantity = this->database_header_->data_blocks_quantity;

	this->data_file_->write(reinterpret_cast<char*>(this->database_header_), sizeof(database_header));

	for (unsigned int i = 0; i < quantity; i++)
	{
		this->data_file_->write(reinterpret_cast<char*>(block), database_header_->data_block_size);
	}

	this->data_file_->close();
	this->data_file_->open(std::ios::in | std::ios::out | std::ios::binary);
}

void engine::create_table(table* table_info) const
{
	const auto position = find_available_data_block('T');
	const auto byte_position = static_cast<int>(std::get<0>(position));
	const auto block_index = static_cast<int>(std::get<1>(position));

	const auto table_block = reinterpret_cast<data_block*>(data_file_->read(byte_position, database_header_->data_block_size));
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

std::tuple<int, int> engine::find_available_data_block(const char block_type) const
{
	auto position = database_header_->first_data_block;

	for (unsigned int i = 0;i < database_header_->bitmap.size(); i++)
	{
		if (database_header_->bitmap[i])
		{
			continue;
		}

		const auto block = reinterpret_cast<data_block*>(data_file_->read(position, database_header_->data_block_size));

		if (block->object_type != 'E' && block->object_type != block_type)
		{
			position += database_header_->data_block_size;
			continue;
		}

		return { position, i };
	}
	return { -1, -1 };
}

std::tuple<unsigned int, unsigned int> engine::create_columns(column* columns_info, const unsigned int columns_quantity) const
{
	unsigned first_block_column_byte_location = 0;
	unsigned first_column_byte_location_in_block = 0;
	for (unsigned int i = 0; i < columns_quantity; i++)
	{
		

		const auto available_data_block_location = find_available_data_block('C');
		const auto byte_location = static_cast<int>(std::get<0>(available_data_block_location));
		const auto column_data_block = reinterpret_cast<data_block*>(data_file_->read(byte_location, database_header_->data_block_size));

		auto current_byte = column_data_block->first_free_byte;

		first_block_column_byte_location = byte_location;
		
		if (i == 0)
		{
			first_column_byte_location_in_block = current_byte;
		}

		memcpy(&column_data_block->data[current_byte], reinterpret_cast<char*>(&columns_info[i]), sizeof column);
		current_byte += sizeof column;

		column_data_block->object_type = 'C';

		data_file_->write(reinterpret_cast<char*>(column_data_block), byte_location, database_header_->data_block_size);
	}

	return { first_block_column_byte_location , first_column_byte_location_in_block };

}
