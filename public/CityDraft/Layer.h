#pragma once

#include <string>

namespace CityDraft
{
	class Scene;

	/// <summary>
	/// Descriptor of a layer
	/// </summary>
	class Layer
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

	private:
		std::string m_Name = "";
		bool m_IsVisible = true;
		bool m_IsLocked = false;
		int m_ZOrder = 0;

		friend class Scene;
	};
}
