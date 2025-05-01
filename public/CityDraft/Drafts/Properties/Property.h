#pragma once

#include <any>
#include <string>
#include <string_view>
#include <functional>
#include <set>
#include <memory>

namespace CityDraft::Drafts
{
	class Draft;
}

namespace CityDraft::Drafts::Properties
{
	class Property
	{
	public:
		template<typename T>
		using GetterFunc = std::function<const T&(CityDraft::Drafts::Draft*)>;

		template<typename T>
		using SetterFunc = std::function<void(CityDraft::Drafts::Draft*, const T&)>;

		template<typename T>
		using ValidatorFunc = std::function<bool(CityDraft::Drafts::Draft*, const T&)>;

		inline Property(std::string_view name, CityDraft::Drafts::Draft* owner) :
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

	protected:
		std::string m_Name;
		CityDraft::Drafts::Draft* m_Owner;
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
}