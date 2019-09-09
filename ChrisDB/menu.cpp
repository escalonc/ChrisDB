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

			std::cout << "Enter the table amount: ";
			std::cin >> table_name;
			std::cout << std::endl << std::endl;

			std::cout << "Enter the columns amount: ";
			std::cin >> columns_amount;
			std::cout << std::endl << std::endl;

			const auto columns_info = new column_dto * [columns_amount];

			for (unsigned i = 0;i < columns_amount;i++)
			{
				char column_name[30]{};
				unsigned column_size = 0;
				char column_data_type{};

				std::cout << "Enter the column name: ";
				std::cin >> column_name;
				std::cout << std::endl << std::endl;

				std::cout << "Enter the columns data type: ";
				std::cin >> column_data_type;
				std::cout << std::endl << std::endl;

				if (column_data_type == 'C')
				{
					std::cout << "Enter the char size: ";
					std::cin >> column_size;
					std::cout << std::endl << std::endl;
				}

				strcpy_s(columns_info[i]->name, 30, table_name);
				columns_info[i]->size = column_size;
				columns_info[i]->data_type = column_data_type;
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
			break;
		}
		case 4:
		{
			char table_name[30]{};

			std::cout << "Enter the table name: ";
			std::cin >> table_name;
			std::cout << std::endl << std::endl;

			const auto table_info = database_engine->find_table_by_name(table_name);
			const auto columns = database_engine->find_columns_of_table(table_info);

				for (unsigned i = 0; i < table_info->columns_amount; i++)
				{
					
				}
				
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
