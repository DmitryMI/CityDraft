#pragma once

#include <any>
#include <string>
#include <string_view>
#include <functional>
#include <set>
#include <memory>
#include <map>
#include <vector>
#include <boost/assert.hpp>
#include <algorithm>
#include <boost/signals2.hpp>

namespace CityDraft::Drafts
{
	class Draft;
}

namespace CityDraft::Drafts::Properties
{
	class Property
	{
	public:
		using ValueChangedSignal = boost::signals2::signal<void()>;

		inline Property(std::string_view name, CityDraft::Drafts::Draft* owner):
			m_Name(name),
			m_Owner(owner)
		{
			BOOST_ASSERT(owner);
		}

		virtual ~Property() = default;

		inline std::string_view GetName() const
		{
			return m_Name;
		}

		inline CityDraft::Drafts::Draft* GetOwner() const
		{
			return m_Owner;
		}

		inline virtual bool IsReadable() const
		{
			return false;
		}

		inline virtual bool IsWritable() const
		{
			return false;
		}

		virtual std::any GetAny() const = 0;
		virtual bool SetAny(std::any value) = 0;

		template<typename T>
		bool TryGet(T& result) const
		{
			std::any valAny = GetAny();
			if(valAny.type() == typeid(T))
			{
				result = std::any_cast<T>(valAny);
				return true;
			}
			return false;
		}

		inline boost::signals2::connection ConnectToValueChanged(ValueChangedSignal::slot_type slot)
		{
			return m_ValueChanged.connect(slot);
		}

	protected:
		std::string m_Name;
		CityDraft::Drafts::Draft* m_Owner;
		ValueChangedSignal m_ValueChanged;

		friend class CityDraft::Drafts::Draft;
	};

	struct PropertyNameComparator
	{
		template<typename TPtr>
		bool operator()(const TPtr a, const TPtr b) const
		{
			return a->GetName() < b->GetName();
		}
	};

	using Set = std::set<std::shared_ptr<Property>, PropertyNameComparator>;
	template<typename T>
	using TVector = std::vector<std::shared_ptr<T>>;
	using Vector = TVector<Property>;
	using Map = std::map<std::string_view, Vector>;

	Map FindCommonProperties(const std::vector<Set>& sets);
}