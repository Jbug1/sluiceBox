#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>

int main()
{
	const std::string filename = "test_input.txt";											//Delcare the filename variable
	auto startWrite = std::chrono::high_resolution_clock::now();							//Set a time checkpoint. Auto is 'auto type' instead of int or string
	std::ofstream myfile;																	//Create an write filestream
	myfile.open(filename);																	//Load the file into the filestream
	for (int i = 0; i < 1000000; ++i) { myfile << "Writing this to a file " << i << "\n"; }	//Add i lines to the file
	myfile.close();																			//Close the input file
	auto endWrite = std::chrono::high_resolution_clock::now();								//Set a time checkpoint

	std::ifstream fileReader(filename, std::ios::binary | std::ios::ate);					//Create an read filestream
	std::string content;																	//Create the string to store the file
	if (fileReader) {																		//Implicit check to see if the filereader is active (if filereader != null)
		std::filesystem::path p{ filename };												//Get the filepath
		auto fileSize = std::filesystem::file_size(p);										//Get the size of the file
		fileReader.seekg(std::ios::beg);													//Go to beginning of filestream
		content = std::string(fileSize, 0);													//Initialize the string's buffer size
		fileReader.read(&content[0], fileSize);												//Load it into the string
	}
	else
	{
		std::cout << "File load error" << std::endl;
	}
	auto endRead = std::chrono::high_resolution_clock::now();

	int linecount = 0;
	for (int i = 0; i < content.size(); ++i)												//For each index in content (the input file), 
	{
		if (content[i] == '\n')																//If the value is a terminating character
		{
			++linecount;																	//Add 1 to linecount
		}
	}
	auto endForLoop = std::chrono::high_resolution_clock::now();

	int linecountB = 0;
	std::ifstream fileReaderB(filename);
	std::string glstring;
	auto startGetLine = std::chrono::high_resolution_clock::now();
	if (fileReaderB) {
		while (std::getline(fileReaderB, glstring))											//Same thing as above, but getting the line instead
		{
			++linecountB;
		}
		fileReaderB.close();
	}
	else
	{
		std::cout << "File load error" << std::endl;
	}
	auto endGetLine = std::chrono::high_resolution_clock::now();

	std::cout << "Time to write 1M lines, and store into string in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(endRead - startWrite).count() << std::endl;
	std::cout << "Time to write 1M lines in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(endWrite - startWrite).count() << std::endl;
	std::cout << "Time to store into string in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(endRead - endWrite).count() << std::endl;
	std::cout << "Time to iterate over " << linecount << " lines: " << std::chrono::duration_cast<std::chrono::milliseconds>(endForLoop - endRead).count() << std::endl;
	std::cout << "Time to run getline on " << linecountB << " lines: " << std::chrono::duration_cast<std::chrono::milliseconds>(endGetLine - startGetLine).count() << std::endl;

	/*
	* Example output
		Time to write 1M lines, and store into string in ms: 4315
		Time to write 1M lines in ms: 4237
		Time to store into string in ms: 77
		Time to iterate over 1000000 lines: 676
		Time to run getline on 1000000 lines: 4787
	*/
	return 0;
}