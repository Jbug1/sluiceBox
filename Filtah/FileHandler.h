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
	uintmax_t filesize = 0;
	int readcount = 0;
};