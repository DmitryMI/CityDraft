#pragma once

namespace CityDraft::Assets
{
	enum class FileSystemError
	{
		Ok,
		FileNotFound,
		NotAFile,
		NotADirectory,
		Unknown
	};
}
