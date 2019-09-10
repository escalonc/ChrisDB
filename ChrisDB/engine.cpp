#include "engine.h"
#include "types.h"
#include <iostream>

engine::engine(char* name)
{
	this->data_file_ = new data_file(name);
	this->database_header_ = new database_header();
	//strcpy_s(database_header_->database_name, strlen(name), name);
}

void engine::create_database(const unsigned database_size, const unsigned data_block_size) const
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

std::tuple<table*, unsigned, unsigned> engine::find_table_by_name(char name[30]) const
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
			const auto table_byte_location_in_block = j * sizeof table;
			memcpy(&buffer[0], &current_data_block->data[table_byte_location_in_block], sizeof table);
			const auto current_table = reinterpret_cast<table*>(buffer);

			if (strcmp(current_table->name, name) == 0)
			{
				return { current_table,  block_position,  table_byte_location_in_block };
			}

			//delete[] buffer;
		}

		//delete current_data_block;
		block_position += database_header_->data_block_size;
	}

	return { nullptr, -1, -1 };
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

std::tuple<int, int> engine::find_available_data_block(const char block_type, const unsigned exclude_position) const
{
	auto position = database_header_->first_data_block;

	for (unsigned int i = 0;i < database_header_->bitmap.size(); i++)
	{
		if (database_header_->bitmap[i])
		{
			continue;
		}

		const auto block = reinterpret_cast<data_block*>(data_file_->read(position, database_header_->data_block_size));

		if (block->object_type != 'E' && block->object_type != block_type && exclude_position != position)
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

		if (i == columns_amount - 1)
		{
			column_data->next_column_byte_location_in_block = -1;
			column_data->next_block_column_byte_location = -1;
		}
		else
		{
			column_data->next_column_byte_location_in_block = first_free_byte + sizeof column;
			column_data->next_block_column_byte_location = byte_location;
		}

		if (first_free_byte < (column_data_block->remaining_space - sizeof column))
		{
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

unsigned engine::get_record_size(table* table_info) const
{
	auto record_size = 0;
	const auto columns = find_columns_of_table(table_info);

	for (unsigned i = 0;i < table_info->columns_amount;i++)
	{
		record_size += columns[i]->size;
	}

	return record_size;
}

void engine::create_record(char* input_buffer, table* table_info, const unsigned block_table_byte_location, unsigned table_byte_location_in_block) const
{
	const auto record_size = this->get_record_size(table_info);
	const auto record_info = new record();
	record_info->data = new char[record_size];
	memcpy(&record_info->data[0], &input_buffer[0], record_size);

	std::tuple<int, int> block_record_position;

	if (table_info->first_block_record_byte_location == -1)
	{
		block_record_position = find_available_data_block('R');
	}
	else
	{
		block_record_position = find_available_data_block('R', table_info->first_block_record_byte_location);
	}

	const auto block_byte_location = std::get<0>(block_record_position);
	const auto block_record = reinterpret_cast<data_block*>(data_file_->read(block_byte_location, sizeof data_block));

	const auto buffer = new char[database_header_->data_block_buffer_size];
	memcpy(&buffer[0], block_record->data, database_header_->data_block_buffer_size);

	if (block_record->first_free_byte < (block_record->remaining_space - sizeof table))
	{
		record_info->next = record_size + sizeof record;
	}

	memcpy(&buffer[block_record->first_free_byte], reinterpret_cast<char*>(record_info), record_size + sizeof record);

	block_record->data = buffer;
	block_record->object_type = 'R';

	block_record->objects_amount += 1;

	data_file_->write(reinterpret_cast<char*>(block_record), block_byte_location, database_header_->data_block_size);

	if (table_info->first_block_record_byte_location == -1)
	{
		table_info->first_block_record_byte_location = int(block_byte_location);
	}

	table_info->last_block_record_byte_location = int(block_byte_location);

	const auto block_table = reinterpret_cast<data_block*>(data_file_->read(block_table_byte_location, database_header_->data_block_size)
	);
	memcpy(&block_table->data[table_byte_location_in_block], reinterpret_cast<char*>(table_info), sizeof table);

	data_file_->write(reinterpret_cast<char*>(block_table), block_table_byte_location, database_header_->data_block_size);

}

void engine::close() const
{
	data_file_->close();
}

void engine::select_all(table* table_info) const
{
	const auto record_size = get_record_size(table_info);
	const auto columns = find_columns_of_table(table_info);
	const auto record_block = reinterpret_cast<data_block*>(data_file_->read(table_info->first_block_record_byte_location, record_size + sizeof record)
		);
	unsigned position = 0;

	for (unsigned i = 0; i < record_block->objects_amount;i++)
	{
		const auto record_buffer = new char[record_size];
		memcpy(&record_buffer[0], &record_block->data[i * (record_size + sizeof record)], record_size + sizeof record);
		std::cout << "-----------------------------------------------------------";
		std::cout << std::endl;

		for (unsigned j = 0; j < table_info->columns_amount; j++)
		{

			if (columns[j]->data_type == 'I')
			{
				int value;
				memcpy(&value, &record_buffer[position], columns[j]->size);
				std::cout << value;

			}
			else if (columns[j]->data_type == 'D')
			{
				double value;
				memcpy(&value, &record_buffer[position], columns[j]->size);
				std::cout << value;
			}
			else
			{
				const auto value = new char[columns[j]->size];
				memcpy(value, &record_buffer[position], columns[j]->size);
				std::cout << value;
			}
			std::cout << std::endl;

			position += columns[j]->size;
		}
	}


	

}
