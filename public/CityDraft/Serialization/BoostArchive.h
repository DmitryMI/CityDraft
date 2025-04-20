#pragma once

#include <string>
#include "CityDraft/Serialization/IArchive.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/Transform2D.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>
#include <filesystem>

namespace CityDraft::Serialization
{
	class BoostInputArchive : public IInputArchive
	{
	public:
		BoostInputArchive(const std::filesystem::path& path);

		IInputArchive& operator>>(int& value) override;
		IInputArchive& operator>>(float& value) override;
		IInputArchive& operator>>(double& value) override;
		IInputArchive& operator>>(std::string& value) override;
		IInputArchive& operator>>(CityDraft::Vector2D& value) override;
		IInputArchive& operator>>(CityDraft::Transform2D& value) override;
		IInputArchive& operator>>(ISerializable& serializable) override;

	private:
		std::ifstream m_Stream;
		boost::archive::text_iarchive m_InArchive;
	};

	class BoostOutputArchive : public IOutputArchive
	{
	public:
		BoostOutputArchive(const std::filesystem::path& path);

		IOutputArchive& operator<<(int value) override;
		IOutputArchive& operator<<(float value) override;
		IOutputArchive& operator<<(double value) override;
		IOutputArchive& operator<<(const std::string& value) override;
		IOutputArchive& operator<<(const CityDraft::Vector2D& value) override;
		IOutputArchive& operator<<(const CityDraft::Transform2D& value) override;
		IOutputArchive& operator<<(const ISerializable& serializable) override;

	private:
		std::ofstream m_Stream;
		boost::archive::text_oarchive m_OutArchive;
		
	};
}
