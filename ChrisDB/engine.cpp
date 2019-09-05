#include "engine.h"
#include "types.h"

engine::engine(char* name)
{
	this->data_file_ = new data_file(name);
	this->database_header_ = new database_header();
	//strcpy_s(database_header_->database_name, strlen(name), name);
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

	const auto block = new data_block();
	block->object_type = 'E';
	block->remaining_space = database_header_->data_block_buffer_size;
	block->data = new char[this->database_header_->data_block_buffer_size];

	const auto quantity = this->database_header_->data_blocks_quantity;

	this->data_file_->write(reinterpret_cast<char*>(this->database_header_), sizeof(database_header));

	for (unsigned int i = 0; i < quantity; i++)
	{
		this->data_file_->write(reinterpret_cast<char*>(block), database_header_->data_block_size);
	}

	//delete block;
	this->data_file_->close();
	this->data_file_->open(std::ios::in | std::ios::out | std::ios::binary);
}

void engine::create_table(char* table_name, column_dto** columns_info, const unsigned int columns_amount) const
{
	const auto column_location = create_columns(columns_info, columns_amount);

	const auto table_info = new table();
	strcpy_s(table_info->name, 30, table_name);
	const auto position = find_available_data_block('T');
	const auto byte_position = static_cast<int>(std::get<0>(position));

	table_info->columns_amount = columns_amount;
	table_info->first_block_column_byte_location = std::get<0>(column_location);
	table_info->first_column_byte_location_in_block = std::get<1>(column_location);

	const auto table_block = reinterpret_cast<data_block*>(data_file_->read(byte_position, database_header_->data_block_size));
	table_block->object_type = 'T';
	table_block->objects_amount++;
	const auto first_byte = table_block->first_free_byte;

	const auto buffer = new char[database_header_->data_block_buffer_size];

	if (first_byte < (table_block->remaining_space - sizeof table))
	{
		memcpy(&buffer[0], &table_block->data[0], database_header_->data_block_buffer_size);

		memcpy(&buffer[first_byte], reinterpret_cast<char*>(table_info), sizeof table);

		table_block->data = buffer;

		table_block->first_free_byte += sizeof table;
		table_block->remaining_space -= sizeof table;
	}

	this->data_file_->write(reinterpret_cast<char*>(table_block), byte_position, database_header_->data_block_size);

	if (database_header_->first_table_block_byte_location == -1)
	{
		database_header_->first_table_block_byte_location = data_file_->write_position() - sizeof table;
	}

	/*delete[] buffer;
	delete table_info;
	delete table_block;*/
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
			block_position += database_header_->data_block_size;
			continue;
		}

		for (unsigned int j = 0; j < current_data_block->objects_amount;j++)
		{
			const auto buffer = new char[sizeof table];

			memcpy(&buffer[0], &current_data_block->data[j * sizeof table], sizeof table);
			const auto current_table = reinterpret_cast<table*>(buffer);

			if (strcmp(current_table->name, name) == 0)
			{
				return current_table;
			}

			//delete[] buffer;
		}

		//delete current_data_block;
		block_position += database_header_->data_block_size;
	}

	return nullptr;
}

column** engine::find_columns_of_table(table* table_info) const
{
	const auto columns = new column * [table_info->columns_amount];

	auto block_location = table_info->first_block_column_byte_location;
	auto column_location_in_block = table_info->first_column_byte_location_in_block;

	for (unsigned i = 0; i < table_info->columns_amount; i++)
	{
		const auto column_block = reinterpret_cast<data_block*>(data_file_->read(block_location, sizeof column));
		const auto buffer = new char[sizeof column];
		memcpy(&buffer[0], &column_block->data[column_location_in_block], sizeof column);
		const auto column_info = reinterpret_cast<column*>(buffer);
		columns[i] = column_info;
		block_location = column_info->next_block_column_byte_location;
		column_location_in_block = column_info->next_column_byte_location_in_block;
		//delete[] buffer;
	}

	return columns;

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

		return { position , i };
	}
	return { -1, -1 };
}

std::tuple<unsigned int, unsigned int> engine::create_columns(column_dto** columns_info, const unsigned int columns_amount) const
{
	unsigned first_block_column_byte_location = 0;
	unsigned first_column_byte_location_in_block = 0;

	for (unsigned int i = 0; i < columns_amount; i++)
	{
		const auto available_data_block_location = find_available_data_block('C');
		const auto byte_location = static_cast<int>(std::get<0>(available_data_block_location));
		const auto column_data_block = reinterpret_cast<data_block*>(data_file_->read(byte_location, database_header_->data_block_size));

		const auto column_data = column_factory(columns_info[i]);

		const auto first_free_byte = column_data_block->first_free_byte;

		first_block_column_byte_location = byte_location;

		if (i == 0)
		{
			first_column_byte_location_in_block = first_free_byte;
		}

		if (first_free_byte < (column_data_block->remaining_space - sizeof column))
		{
			column_data->next_column_byte_location_in_block = first_free_byte + sizeof column;
			column_data->next_block_column_byte_location = byte_location;

			memcpy(&column_data_block->data[first_free_byte], reinterpret_cast<char*>(column_data), sizeof column);

			column_data_block->first_free_byte += sizeof column;
			column_data_block->remaining_space -= sizeof column;
		}

		column_data_block->object_type = 'C';

		data_file_->write(reinterpret_cast<char*>(column_data_block), byte_location, database_header_->data_block_size);
	}

	//delete[] columns_data;

	return { first_block_column_byte_location , first_column_byte_location_in_block };
}

column* engine::column_factory(column_dto* column_info)
{
	const auto column_data = new column();
	strcpy_s(column_data->name, 30, column_info->name);
	column_data->data_type = column_info->data_type;

	if (column_info->data_type == 'I')
	{
		column_data->size = 4;
	}
	else if (column_info->data_type == 'D') { column_data->size = 8; }
	else { column_data->size = column_info->size; }

	return column_data;
}
