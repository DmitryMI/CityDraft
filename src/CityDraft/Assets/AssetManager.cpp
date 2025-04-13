#include "CityDraft/Assets/AssetManager.h"
#include <fstream>
#include <boost/throw_exception.hpp>
#include <boost/assert.hpp>

namespace CityDraft::Assets
{
	AssetManager::AssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger):
		m_AssetsRoot(std::filesystem::absolute(assetsRoot)),
		m_Logger(logger)
	{
		BOOST_ASSERT(!assetsRoot.empty());
		BOOST_ASSERT(logger);
	}

	size_t AssetManager::LoadAssets(const std::filesystem::path& assetsDir, bool createMissingDirs)
	{
		BOOST_ASSERT(!assetsDir.empty());

		std::filesystem::path assetsDirRelative = std::filesystem::relative(assetsDir, m_AssetsRoot);

		m_Logger->info("Loading assets from {} ({})...", assetsDir.string(), assetsDirRelative.string());

		size_t loadedNum = 0;
		if (!std::filesystem::exists(assetsDirRelative))
		{
			std::filesystem::create_directories(assetsDirRelative);
		}

		std::filesystem::path imagesDir = assetsDirRelative / ImagesDir;
		if (!std::filesystem::exists(imagesDir))
		{
			if (createMissingDirs)
			{
				std::filesystem::create_directories(imagesDir);
			}
		}

		loadedNum += LoadImageAssets(imagesDir);

		m_Logger->info("Loaded {} assets from {}", loadedNum, assetsDirRelative.string());
		return loadedNum;
	}

	size_t AssetManager::LoadImageAssets(const std::filesystem::path& imagesDir)
	{
		BOOST_ASSERT(!imagesDir.empty());

		m_Logger->info("Loading Images from {}", imagesDir.string());

		size_t loadedNum = 0;
		if (!std::filesystem::is_directory(imagesDir))
		{
			m_Logger->info("Image directory {} is empty or does not exist", imagesDir.string());
			return 0;
		}

		for (const auto& entry : std::filesystem::directory_iterator(imagesDir))
		{
			if (entry.is_directory())
			{
				loadedNum += LoadVariantImageGroup(entry);
			}
		}

		m_Logger->info("Loaded {} images from {}", loadedNum, imagesDir.string());
		return loadedNum;
	}

	size_t AssetManager::LoadVariantImageGroup(const std::filesystem::path& variantDir)
	{
		m_Logger->info("Loading Image Variant Group from {}", variantDir.string());

		std::list<ImageAsset*> images;

		for (const auto& entry : std::filesystem::directory_iterator(variantDir))
		{
			if (entry.is_directory())
			{
				m_Logger->warn("Ignoring unexpected directory {} inside Image Variant Group {}", entry.path().string(), variantDir.string());
				continue;
			}

			ImageAsset* image = new ImageAsset(entry.path());
			images.push_back(image);
		}

		std::shared_ptr<ImageVariantGroup> group = std::make_shared<ImageVariantGroup>(images);
		m_VariantImages.push_back(group);
		return images.size();
	}

	const std::list<std::shared_ptr<ImageVariantGroup>>& AssetManager::GetVariantImages() const
	{
		return m_VariantImages;
	}

	const std::list<std::shared_ptr<ImageAsset>>& AssetManager::GetInvariantImages() const
	{
		return m_InvariantImages;
	}

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

	boost::url AssetManager::AssetRelativePathToUrl(const std::filesystem::path& path)
	{
		BOOST_ASSERT(path.is_relative());

		boost::system::result<boost::url_view> uriParseResult;

		std::string genericString = path.generic_string();
		std::string urlString = "file://" + std::string("assets/") + genericString;
		uriParseResult = boost::urls::parse_uri(urlString);
		if (uriParseResult.has_value())
		{
			return uriParseResult.value();
		}

		return boost::url();
	}

	std::filesystem::path AssetManager::ToAssetRelativePath(const boost::url& url)
	{
		BOOST_ASSERT(url.scheme() == "file");
		BOOST_ASSERT(url.host() == "assets");
		std::string pathStr = url.path();
		BOOST_ASSERT(pathStr[0] == '/');
		pathStr.erase(pathStr.begin());
		std::filesystem::path path(pathStr);
		BOOST_ASSERT(path.is_relative());
		return path;
	}
}
