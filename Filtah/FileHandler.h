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
	~FileHandler();
	void ProcessNextChunk();

	std::string content;
	int readcount = 0;

	uintmax_t cursor_start = 0;
	uintmax_t cursor_end = 0;


	inline uintmax_t GetRealSize() { return calcedFileSize; }

	std::ifstream fileReader;

	const uintmax_t STRINGBUFFER_SIZE = 1000000000;
	bool singleRead = false;
private:
	//file info
	uintmax_t filesize = 0;
	uintmax_t filesizeIter = 0;
	uintmax_t calcedFileSize = 0;
};