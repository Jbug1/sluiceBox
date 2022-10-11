#include "FileHandler.h"

FileHandler::FileHandler(std::string transcriptome)
{
	std::ifstream fileReader(transcriptome, std::ios::binary);
	if (fileReader) {
		std::filesystem::path p{ transcriptome };
		filesize = std::filesystem::file_size(p);
		fileReader.seekg(cursor, std::ios::beg);

		std::cout << "FILE SIZE: " << filesize << std::endl;
		content = std::string(filesize, 0);
		fileReader.read(&content[0], filesize);
		fileReader.close();

		for (int i = 0; i < content.size(); ++i)
		{
			if (content[i] == '+') { ++readcount; }
		}
	}
	else
	{
		std::cout << "File load error with: " << transcriptome << std::endl;
		abort();
	}
}