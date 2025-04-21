#pragma once

#include "CityDraft/Serialization/BoostArchive.h"

namespace CityDraft::Serialization
{

	BoostInputArchive::BoostInputArchive(const std::filesystem::path& path):
		m_Stream(path),
		m_InArchive(m_Stream)
	{
	}

	IInputArchive& BoostInputArchive::operator>>(int32_t& value)
	{
		m_InArchive >> value;
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(int64_t& value)
	{
		m_InArchive >> value;
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(size_t& value)
	{
		m_InArchive >> value;
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(float& value)
	{
		m_InArchive >> value;
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(double& value)
	{
		m_InArchive >> value;
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(std::string& value)
	{
		m_InArchive >> value;
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(CityDraft::Vector2D& value)
	{
		double x, y;
		m_InArchive >> x;
		m_InArchive >> y;
		value.SetX(x);
		value.SetY(y);
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(CityDraft::Transform2D& value)
	{
		*this >> value.Translation;
		*this >> value.Rotation;
		*this >> value.Scale;
		return *this;
	}

	IInputArchive& BoostInputArchive::operator>>(ISerializable& serializable)
	{
		serializable.Deserialize(*this);
		return *this;
	}

	BoostOutputArchive::BoostOutputArchive(const std::filesystem::path& path):
		m_Stream(path),
		m_OutArchive(m_Stream)
	{
	}

	IOutputArchive& BoostOutputArchive::operator<<(int32_t value)
	{
		m_OutArchive << value;
		return *this;
	}

	IOutputArchive& BoostOutputArchive::operator<<(int64_t value)
	{
		m_OutArchive << value;
		return *this;
	}


	IOutputArchive& BoostOutputArchive::operator<<(size_t value)
	{
		m_OutArchive << value;
		return *this;
	}

	IOutputArchive& BoostOutputArchive::operator<<(float value)
	{
		m_OutArchive << value;
		return *this;
	}

	IOutputArchive& BoostOutputArchive::operator<<(double value)
	{
		m_OutArchive << value;
		return *this;
	}

	IOutputArchive& BoostOutputArchive::operator<<(const std::string& value)
	{
		m_OutArchive << value;
		return *this;
	}

	IOutputArchive& BoostOutputArchive::operator<<(const CityDraft::Vector2D& value)
	{
		m_OutArchive << value.GetX();
		m_OutArchive << value.GetY();
		return *this;
	}

	IOutputArchive& BoostOutputArchive::operator<<(const CityDraft::Transform2D& value)
	{
		*this << value.Translation;
		*this << value.Rotation;
		*this << value.Scale;
		return *this;
	}

	IOutputArchive& BoostOutputArchive::operator<<(const ISerializable& serializable)
	{
		serializable.Serialize(*this);
		return *this;
	}
}
