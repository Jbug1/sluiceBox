#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <cassert>

class FileHandler
{
public:
	FileHandler(std::string transcriptome);
	std::string content;
	int readcount = 0;

	int cursor = 0;

	//file info
	uintmax_t filesize = 0;
};