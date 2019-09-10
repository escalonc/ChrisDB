#include "menu.h"
#include <iostream>
#include "engine.h"

void menu::init()
{
	auto response = 'n';
	char database_name[30];
	unsigned option{};
	unsigned database_size{};
	unsigned data_block_size{};

	header();

	std::cout << "Enter the database name: ";
	std::cin >> database_name;
	std::cout << std::endl;

	std::cout << "Enter the database size: ";
	std::cin >> database_size;

	std::cout << "Enter the data block size: ";
	std::cin >> data_block_size;
	std::cout << std::endl;

	const auto database_engine = new engine(database_name);
	database_engine->create_database(database_size, data_block_size);

	system("cls");
	do
	{
		system("cls");
		std::cout << "*********************" << std::endl;
		std::cout << "* 1) Create table   *" << std::endl;
		std::cout << "* 2) List tables    *" << std::endl;
		std::cout << "* 3) Select records *" << std::endl;
		std::cout << "* 4) Insert records *" << std::endl;
		std::cout << "* 5) Update records *" << std::endl;
		std::cout << "* 6) Drop database  *" << std::endl;
		std::cout << "*********************" << std::endl;
		std::cout << std::endl << std::endl;
		std::cout << "Enter an option: ";
		std::cin >> option;

		switch (option)
		{
		case 1:
		{
			char table_name[30];
			unsigned columns_amount;

			std::cout << "Enter the table name: ";
			std::cin >> table_name;
			std::cout << std::endl;

			std::cout << "Enter the columns amount: ";
			std::cin >> columns_amount;
			std::cout << std::endl << std::endl;

			const auto columns_info = new column_dto * [columns_amount];

			for (unsigned i = 0;i < columns_amount;i++)
			{
				const auto current_column = new column_dto();
				char column_name[30]{};
				unsigned column_size = 0;
				char column_data_type{};

				std::cout << "Enter the column name: ";
				std::cin >> column_name;
				std::cout << std::endl;

				std::cout << "Enter the columns data type: ";
				std::cin >> column_data_type;
				std::cout << std::endl;

				if (column_data_type == 'C')
				{
					std::cout << "Enter the char size: ";
					std::cin >> column_size;
					std::cout << std::endl << std::endl;
				}

				strncpy_s(current_column->name, column_name, 30);
				current_column->size = column_size;
				current_column->data_type = column_data_type;

				columns_info[i] = current_column;
			}


			database_engine->create_table(table_name, columns_info, columns_amount);

			break;
		}
		case 2:
		{

			break;
		}
		case 3:
		{
			char table_name[30];
			

			std::cout << "Enter the table name: ";
			std::cin >> table_name;
			std::cout << std::endl;

			const auto table_data = database_engine->find_table_by_name(table_name);
			const auto table_info = static_cast<table*>(std::get<0>(table_data));

			database_engine->select_all(table_info);
				
			break;
		}
		case 4:
		{
			char table_name[30]{};
			unsigned first_free_byte = 0;

			std::cout << "Enter the table name: ";
			std::cin >> table_name;
			std::cout << std::endl << std::endl;

			const auto table_data = database_engine->find_table_by_name(table_name);

			const auto table_info = static_cast<table*>(std::get<0>(table_data));
			const auto block_table_byte_location = static_cast<unsigned>(std::get<1>(table_data));
			const auto table_byte_location_in_block = static_cast<unsigned>(std::get<2>(table_data));

			const auto columns = database_engine->find_columns_of_table(table_info);

			const auto input_buffer = new char[database_engine->get_record_size(table_info)];

			for (unsigned i = 0; i < table_info->columns_amount; i++)
			{
				std::cout << "Enter the the value for field " << columns[i]->name << ": ";

				if (columns[i]->data_type == 'I')
				{
					int value;
					std::cin >> value;
					memcpy(&input_buffer[first_free_byte], reinterpret_cast<char*>(&value), columns[i]->size);

				}
				else if (columns[i]->data_type == 'D')
				{
					double value;
					std::cin >> value;
					memcpy(&input_buffer[first_free_byte], reinterpret_cast<char*>(&value), columns[i]->size);
				}
				else
				{
					const auto value = new char[columns[i]->size];
					std::cin >> value;
					memcpy(&input_buffer[first_free_byte], reinterpret_cast<char*>(value), columns[i]->size);
				}

				first_free_byte += columns[i]->size;
			}

			database_engine->create_record(input_buffer, table_info, block_table_byte_location, table_byte_location_in_block);

			break;
		}
		case 5:
		{
			break;
		}
		case 6:
		{
			auto status = -1;
			database_engine->close();
			status = remove(database_name);

			if (status == 0)
			{
				std::cout << "Database deleted successfully!";
			}
			else
			{
				std::cout << "There was an error when trying to delete the database!";
			}
			break;
		}
		default:
		{
			break;
		}
		}

		std::cout << "continue? ";

		std::cin >> response;
	} while (response == 'y');
	std::cout << std::endl << std::endl;


}

void menu::header()
{
	std::cout << "Chris' database engine" << std::endl << std::endl;
}
