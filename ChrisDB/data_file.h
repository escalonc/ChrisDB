#pragma once

#include <fstream>

class data_file
{
	std::fstream* file_;
	const char* path_;
	
public:
	data_file();
	~data_file();
	explicit data_file(const char* path);
	void open() const;
	void open(std::ios_base::openmode mode) const;
	void close() const;
	void write(char* data, unsigned int position, unsigned int size) const;
	void write(char* data, unsigned int size) const;
	char* read(unsigned int position, unsigned int size) const;
	char* read(unsigned int size) const;
	long size() const;
	void read_position(int position) const;
	long write_position() const;
	long read_position() const;
};
