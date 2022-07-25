#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>

int main()
{
	std::ofstream myfile;
	myfile.open("test_input.txt");
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 1000000000; ++i)
	{
		myfile << "Writing this to a file " << i << "\n";
	}
	myfile.close();

	std::cout << "Starting..." << std::endl;


	std::ifstream fileReader("test_input.txt", std::ios::binary | std::ios::ate);
	std::string content;
	if (fileReader) {
		std::filesystem::path p{"test_input.txt"};
		auto fileSize = std::filesystem::file_size(p);
		fileReader.seekg(std::ios::beg);
		content = std::string(fileSize, 0);
		fileReader.read(&content[0], fileSize);
		std::cout << fileSize << std::endl;
	}
	else
	{
		std::cout << "File load error" << std::endl;
	}
	auto stop = std::chrono::high_resolution_clock::now();

	//std::cout << content << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << std::endl;

	return 0;
}