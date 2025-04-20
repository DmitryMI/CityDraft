#pragma once

#include <string>


namespace CityDraft::Serialization
{
	class IInputArchive;
	class IOutputArchive;

	class ISerializable
	{
	public:
		virtual void Serialize(IOutputArchive& archive) const = 0;
		virtual void Deserialize(IInputArchive& archive) = 0;
	};
}
