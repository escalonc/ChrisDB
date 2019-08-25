#include "engine.h"
#include "types.h"

engine::engine(char* name)
{
	this->data_file_ = new data_file(name);
	this->database_header_ = new database_header();
	// strcat_s(this->database_name_, 30, this->database_name_);
}

void engine::create_database(const int database_size, const int data_block_size) const
{
	this->data_file_->open(std::ios::in | std::ios::out | std::ios::app | std::ios::binary);

	const auto quantity_from_mb_to_b = 1024 * 1024 * database_size;
	this->database_header_->data_blocks_quantity = (quantity_from_mb_to_b - sizeof(database_header)) / data_block_size;
	this->database_header_->net_data_block_size = sizeof(data_block) + data_block_size;
	this->database_header_->data_block_size = data_block_size;
	this->database_header_->first_data_block = sizeof(database_header);
	// strcat_s(this->database_header_->database_name, 30, this->database_name_);


	const auto block = new data_block();
	block->data = new char[this->database_header_->net_data_block_size];

	const auto quantity = this->database_header_->data_blocks_quantity;
	const auto block_size = this->database_header_->net_data_block_size;

	this->data_file_->write(reinterpret_cast<char*>(this->database_header_), sizeof(database_header));
	
	for (unsigned int i = 0; i < quantity; i++)
	{
		this->data_file_->write(reinterpret_cast<char*>(block), block_size);
	}
	this->data_file_->close();
	this->data_file_->open(std::ios::in | std::ios::out | std::ios::binary);
}

void engine::create_table(table* table_info) const
{
	const auto table_block = new data_block();
	table_block->data = new char[database_header_->data_block_size];
	table_block->object_type = 'T';
	const auto first_byte = table_block->first_free_byte;
	memcpy(&table_block->data[0], reinterpret_cast<char*>(table_info), sizeof table);
	
	
	this->data_file_->write(reinterpret_cast<char*>(table_block), this->database_header_->first_data_block, database_header_->first_data_block);
}

table* engine::read_table() const
{
	const auto table_block = reinterpret_cast<data_block*>(data_file_->read(database_header_->first_data_block,
	                                                                  database_header_->data_block_size));
	table* table_info{};
	auto buffer = new char[database_header_->data_block_size];
	memcpy(buffer, &table_block->data[0], sizeof table);
	table_info = reinterpret_cast<table*>(buffer);
	return table_info;
}
