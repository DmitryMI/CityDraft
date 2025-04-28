#pragma once

#include <string>
#include <map>
#include "CityDraft/Serialization/ISerializable.h"
#include "CityDraft/Serialization/IArchive.h"

namespace CityDraft
{
	class Scene;
	namespace Drafts
	{
		class Draft;
	}

	/// <summary>
	/// Descriptor of a layer
	/// </summary>
	class Layer : public Serialization::ISerializable
	{
	public:
		
		inline bool IsVisible() const
		{
			return m_IsVisible;
		}

		inline bool IsLocked() const
		{
			return m_IsLocked;
		}

		inline const std::string& GetName() const
		{
			return m_Name;
		}

		inline int64_t GetZOrder() const
		{
			return m_ZOrder;
		}

	private:
		std::string m_Name = "";
		bool m_IsVisible = true;
		bool m_IsLocked = false;
		int64_t m_ZOrder = 0;

		std::map<int64_t, CityDraft::Drafts::Draft*> m_Drafts;

		Layer() = default;

		inline Layer(const std::string& name):
			m_Name(name),
			m_ZOrder(0)
		{

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

		static inline std::shared_ptr<Layer> Make(const std::string& name);
		static inline std::shared_ptr<Layer> Make();

		friend class Scene;
		struct MakeSharedEnabler;
	};

	struct Layer::MakeSharedEnabler: public Layer
	{
		MakeSharedEnabler(): Layer()
		{}

		MakeSharedEnabler(const std::string& name): Layer(name)
		{}
	};

	std::shared_ptr<Layer> Layer::Make(const std::string& name)
	{
		return std::make_shared<MakeSharedEnabler>(name);
	}

	std::shared_ptr<Layer> Layer::Make()
	{
		return std::make_shared<MakeSharedEnabler>();
	}
}
