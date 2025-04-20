#pragma once

#include <string>
#include "ISerializable.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/Transform2D.h"

namespace CityDraft::Serialization
{
	class IInputArchive
	{
	public:
		virtual IInputArchive& operator>>(int& value) = 0;
		virtual IInputArchive& operator>>(float& value) = 0;
		virtual IInputArchive& operator>>(double& value) = 0;
		virtual IInputArchive& operator>>(std::string& value) = 0;
		virtual IInputArchive& operator>>(CityDraft::Vector2D& value) = 0;
		virtual IInputArchive& operator>>(CityDraft::Transform2D& value) = 0;
		virtual IInputArchive& operator>>(ISerializable& serializable) = 0;
	};

	class IOutputArchive
	{
	public:
		virtual IOutputArchive& operator<<(int value) = 0;
		virtual IOutputArchive& operator<<(float value) = 0;
		virtual IOutputArchive& operator<<(double value) = 0;
		virtual IOutputArchive& operator<<(const std::string& value) = 0;
		virtual IOutputArchive& operator<<(const CityDraft::Vector2D& value) = 0;
		virtual IOutputArchive& operator<<(const CityDraft::Transform2D& value) = 0;
		virtual IOutputArchive& operator<<(const ISerializable& serializable) = 0;
	};

}
