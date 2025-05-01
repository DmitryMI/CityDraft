#pragma once

#include <string>
#include "ISerializable.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/Transform2D.h"
#include "CityDraft/LinearColor.h"
#include <cstdint>

namespace CityDraft::Serialization
{
	class IInputArchive
	{
	public:
		virtual IInputArchive& operator>>(bool& value) = 0;
		virtual IInputArchive& operator>>(int32_t& value) = 0;
		virtual IInputArchive& operator>>(int64_t& value) = 0;
		virtual IInputArchive& operator>>(size_t& value) = 0;
		virtual IInputArchive& operator>>(float& value) = 0;
		virtual IInputArchive& operator>>(double& value) = 0;
		virtual IInputArchive& operator>>(std::string& value) = 0;
		virtual IInputArchive& operator>>(CityDraft::Vector2D& value) = 0;
		virtual IInputArchive& operator>>(CityDraft::Transform2D& value) = 0;
		virtual IInputArchive& operator>>(CityDraft::LinearColorF& value) = 0;
		virtual IInputArchive& operator>>(ISerializable& serializable) = 0;
	};

	class IOutputArchive
	{
	public:
		virtual IOutputArchive& operator<<(bool value) = 0;
		virtual IOutputArchive& operator<<(int32_t value) = 0;
		virtual IOutputArchive& operator<<(int64_t value) = 0;
		virtual IOutputArchive& operator<<(size_t value) = 0;
		virtual IOutputArchive& operator<<(float value) = 0;
		virtual IOutputArchive& operator<<(double value) = 0;
		virtual IOutputArchive& operator<<(const std::string& value) = 0;
		virtual IOutputArchive& operator<<(const CityDraft::Vector2D& value) = 0;
		virtual IOutputArchive& operator<<(const CityDraft::Transform2D& value) = 0;
		virtual IOutputArchive& operator<<(const CityDraft::LinearColorF& value) = 0;
		virtual IOutputArchive& operator<<(const ISerializable& serializable) = 0;
	};

}
