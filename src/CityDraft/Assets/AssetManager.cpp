#include "CityDraft/Assets/AssetManager.h"
#include <fstream>
#include <boost/throw_exception.hpp>
#include <boost/assert.hpp>
#include "CityDraft/Assets/ColorCurve.h"

namespace CityDraft::Assets
{
	AssetManager::AssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger):
		m_AssetsRoot(std::filesystem::absolute(assetsRoot)),
		m_Logger(logger)
	{
		BOOST_ASSERT(!assetsRoot.empty());
		BOOST_ASSERT(logger);

		m_Logger->info("AssetManager initialized");
	}

	std::shared_ptr<Asset> AssetManager::GetByUrl(const boost::url& url)
	{
		if(m_ColorCurve->GetUrl() == url)
		{
			return m_ColorCurve;
		}

		for (const auto& variantImage : m_VariantImages)
		{
			for (const auto& variant : variantImage->GetImageVariants())
			{
				if (variant->GetUrl() == url)
				{
					return variant;
				}
			}
		}

		for (const auto& image : m_InvariantImages)
		{
			if (image->GetUrl() == url)
			{
				return image;
			}
		}

		m_Logger->error("Asset {} was not found!", url.c_str());

		return nullptr;
	}

	std::shared_ptr<Asset> AssetManager::GetByUrl(const std::string& urlStr)
	{
		auto urlParse = boost::urls::parse_uri(urlStr);
		if (!urlParse.has_value())
		{
			m_Logger->error("GetByUrl: URL parsing failed");
			return nullptr;
		}

		return GetByUrl(urlParse.value());
	}

	size_t AssetManager::LoadAssetInfos(const std::filesystem::path& assetsDir, bool createMissingDirs)
	{
		BOOST_ASSERT(!assetsDir.empty());

		std::filesystem::path assetsDirAbsolute = std::filesystem::absolute(assetsDir);

		m_Logger->info("Looking for assets in {} ({})...", assetsDir.string(), assetsDirAbsolute.string());

		size_t loadedNum = 0;
		if (!std::filesystem::exists(assetsDirAbsolute))
		{
			std::filesystem::create_directories(assetsDirAbsolute);
		}

		std::filesystem::path imagesDirRelative = assetsDirAbsolute / ImagesDir;
		std::filesystem::path imagesDirAbsolute = assetsDirAbsolute / ImagesDir;
		if (!std::filesystem::exists(imagesDirAbsolute))
		{
			if (createMissingDirs)
			{
				std::filesystem::create_directories(imagesDirAbsolute);
			}
		}

		loadedNum += LoadImageInfos(imagesDirAbsolute);
		
		m_ColorCurve = CreateColorCurve();

		m_Logger->info("Found {} assets in {}", loadedNum, assetsDirAbsolute.string());
		return loadedNum;
	}

	size_t AssetManager::LoadImageInfos(const std::filesystem::path& imagesDir)
	{
		BOOST_ASSERT(!imagesDir.empty());

		m_Logger->info("Looking for Images in {}", imagesDir.string());

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
				loadedNum += LoadVariantImageGroupInfo(entry);
			}
			else
			{
				auto image = CreateImage(entry);
				if (image)
				{
					loadedNum++;
					m_InvariantImages.push_back(image);
					m_Logger->info("Found invariant image {}", entry.path().string());
				}
			}

		}

		m_Logger->info("Found in total {} images in {}", loadedNum, imagesDir.string());
		return loadedNum;
	}

	size_t AssetManager::LoadVariantImageGroupInfo(const std::filesystem::path& variantDir)
	{
		std::list<std::shared_ptr<Image>> images;

		for (const auto& entry : std::filesystem::directory_iterator(variantDir))
		{
			if (entry.is_directory())
			{
				m_Logger->warn("Ignoring unexpected directory {} inside Image Variant Group {}", entry.path().string(), variantDir.string());
				continue;
			}

			auto image = CreateImage(entry);
			if (image)
			{
				images.push_back(image);
			}
		}

		m_Logger->info("Found image variant group in {} ({} images)", variantDir.string(), images.size());
		std::shared_ptr<ImageVariantGroup> group = std::make_shared<ImageVariantGroup>(images);
		m_VariantImages.push_back(group);
		return images.size();
	}

	const std::list<std::shared_ptr<ImageVariantGroup>>& AssetManager::GetVariantImages() const
	{
		return m_VariantImages;
	}

	const std::list<std::shared_ptr<Image>>& AssetManager::GetInvariantImages() const
	{
		return m_InvariantImages;
	}

	const std::list<std::shared_ptr<Curve>>& AssetManager::GetCurves() const
	{
		return m_Curves;
	}

	std::shared_ptr<ColorCurve> AssetManager::GetColorCurve() const
	{
		return m_ColorCurve;
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

	boost::url AssetManager::AssetPathToUrl(const std::filesystem::path& path)
	{
		BOOST_ASSERT(path.is_absolute());
		std::filesystem::path pathRel = std::filesystem::relative(path, m_AssetsRoot);

		std::string genericString = pathRel.generic_string();
		
		boost::url url;
		url.set_scheme("file");
		url.set_host("assets");
		url.set_encoded_path(genericString);

		return url;
	}

	std::filesystem::path AssetManager::ToAssetPath(const boost::url& url)
	{
		BOOST_ASSERT(url.scheme() == "file");
		BOOST_ASSERT(url.host() == "assets");
		std::string pathStr = url.path();
		BOOST_ASSERT(pathStr[0] == '/');
		pathStr.erase(pathStr.begin());
		std::filesystem::path path(pathStr);
		std::filesystem::path pathAbs = m_AssetsRoot / path;

		return pathAbs.lexically_normal();
	}
}
