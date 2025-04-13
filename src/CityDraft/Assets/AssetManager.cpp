#include "CityDraft/Assets/AssetManager.h"
#include <fstream>

namespace CityDraft::Assets
{
	FileSystemError AssetManager::ReadFileBytes(const std::filesystem::path& filePath, std::vector<uint8_t>& outBytes)
	{

		if (!std::filesystem::exists(filePath))
		{
			return FileSystemError::FileNotFound;
		}

		if (!std::filesystem::is_regular_file(filePath))
		{
			return FileSystemError::NotAFile;
		}

		std::ifstream file(filePath, std::ios::binary | std::ios::ate);

		if (!file)
		{
			return FileSystemError::Unknown;
		}

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		outBytes.resize(size);
		if (!file.read(reinterpret_cast<char*>(outBytes.data()), size))
		{
			return FileSystemError::Unknown;
		}

		return FileSystemError::Ok;
	}
}
