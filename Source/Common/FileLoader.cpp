#include <Link/Common/FileLoader.hpp>

using namespace Link;

bool FileLoader::LoadFromFile(std::string path, std::string* content)
{
	std::string source = "";

	std::ifstream file;
	file.open(path);

	//Check if the source failed to load
	if (!file)
		return false;

	std::string line;
	while (getline(file, line))
		source += line + "\n";

	file.close();

	*content = source;

    return true;
}
