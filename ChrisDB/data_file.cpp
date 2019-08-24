#include "data_file.h"

data_file::data_file()
{
	this->file_ = new std::fstream();
	this->path_ = nullptr;
}

data_file::data_file(const char* path)
{
	this->path_ = path;
	this->file_ = new std::fstream();
}

data_file::~data_file()
{
	delete this->file_;
	delete[] this->path_;
}

void data_file::open() const
{
	this->file_->open(this->path_, std::ios::in | std::ios::out | std::ios::binary);
}

void data_file::open(std::ios_base::openmode mode) const
{
	this->file_->open(this->path_, mode);
}

void data_file::close() const { this->file_->close(); }

void data_file::write(char* data, unsigned int position, unsigned int size) const
{
	this->file_->seekp(position, std::ios::beg);

	this->file_->write(data, size);
	this->file_->flush();
}

void data_file::write(char* data, unsigned int size) const
{
	this->file_->write(data, size);
	this->file_->flush();
}

char* data_file::read(unsigned int position, unsigned int size) const
{
	char* element = new char[size];

	this->file_->seekg(position, std::ios::beg);
	if (this->file_->read(element, size))
	{
		return element;
	}
}

char* data_file::read(unsigned int size) const
{
	char* element = new char[size];

	if (this->file_->read(element, size))
	{
		return element;
	}
}

long data_file::write_position() const { return this->file_->tellp(); }

long data_file::read_position() const
{
	return this->file_->tellg();
}

void data_file::read_position(int position) const
{
	this->file_->seekg(position, std::ios::beg);
}

long data_file::size()
{
	long previousPosition = this->read_position();
	this->file_->seekg(0, std::ios::end);
	int newPosition = this->read_position();
	this->read_position(previousPosition);
	return newPosition;
}