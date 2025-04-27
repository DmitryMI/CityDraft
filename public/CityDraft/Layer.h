#pragma once

#include <string>
#include "CityDraft/Serialization/ISerializable.h"
#include "CityDraft/Serialization/IArchive.h"

namespace CityDraft
{
	class Scene;

	/// <summary>
	/// Descriptor of a layer
	/// </summary>
	class Layer : public Serialization::ISerializable
	{
	public:
		Layer() = default;

		inline Layer(const std::string& name, int zOrder):
			m_Name(name),
			m_ZOrder(zOrder)
		{

		}

		inline bool IsVisible() const
		{
			return m_IsVisible;
		}

		inline void SetVisible(bool visible)
		{
			m_IsVisible = visible;
		}

		inline bool IsLocked() const
		{
			return m_IsLocked;
		}

		inline void SetLocked(bool locked)
		{
			m_IsLocked = locked;
		}

		inline const std::string& GetName() const
		{
			return m_Name;
		}

		inline void SetName(const std::string& name)
		{
			m_Name = name;
		}

		inline int GetZOrder() const
		{
			return m_ZOrder;
		}

		inline void SetZOrder(int zOrder)
		{
			m_ZOrder = zOrder;
		}

		// ISerializable
		inline void Serialize(Serialization::IOutputArchive& archive) const override
		{
			archive << m_Name;
			archive << m_IsVisible;
			archive << m_IsLocked;
			archive << m_ZOrder;
		}

		inline void Deserialize(Serialization::IInputArchive& archive) override
		{
			archive >> m_Name;
			archive >> m_IsVisible;
			archive >> m_IsLocked;
			archive >> m_ZOrder;
		}

	private:
		std::string m_Name = "";
		bool m_IsVisible = true;
		bool m_IsLocked = false;
		int m_ZOrder = 0;

		friend class Scene;
	};
}
