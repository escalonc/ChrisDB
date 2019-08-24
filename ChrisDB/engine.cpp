#include "engine.h"
#include "types.h"

engine::engine(char* name)
{
	this->data_file_ = new data_file(name);
	this->database_header_ = new database_header();
}

void engine::create_database(const int database_size, const int data_block_size) const
{
	this->data_file_->open(std::ios::in | std::ios::out | std::ios::app);

	const auto quantity_from_mb_to_b = 1024 * 1024 * database_size;
	this->database_header_->data_blocks_quantity = (quantity_from_mb_to_b - sizeof(database_header)) / data_block_size;
	this->database_header_->data_block_size = data_block_size;


	const auto block = new data_block();
	block->data = new char[this->database_header_->data_block_size];

	const auto quantity = this->database_header_->data_blocks_quantity;
	const auto block_size = sizeof(data_block) + this->database_header_->data_block_size;

	this->data_file_->write(reinterpret_cast<char*>(this->database_header_), sizeof(database_header));
	for (auto i = 0; i < quantity; i++)
	{
		this->data_file_->write(reinterpret_cast<char*>(block), block_size);
	}
}
