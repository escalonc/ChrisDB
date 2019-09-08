#include "menu.h"
#include <iostream>

void menu::init()
{
	auto response = 'n';
	char name[30];
	int option;

	std::cout << "Enter the database name: ";
	std::cin >> name;
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
		std::cout << "Enter an option";

		std::cin >> option;

		switch (option)
		{
		case 1:
		{
			break;
		}
		case 2:
		{
			break;
		}
		default:
		{
			break;
		}
		}
	} while (response == 'y');
	std::cout << std::endl << std::endl;


}

void menu::header()
{
	std::cout << "Chris' database engine" << std::endl << std::endl;
}
