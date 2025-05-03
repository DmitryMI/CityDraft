#pragma once

#include "TypedProperty.h"
#include <string_view>
#include <cstdint>

namespace CityDraft::Drafts::Properties
{
	template<typename T>
	class View : public TypedProperty<T>
	{
	public:
		template<typename T>
		using GetterFunc = std::function<const T& (CityDraft::Drafts::Draft*)>;

		template<typename T>
		using SetterFunc = std::function<void(CityDraft::Drafts::Draft*, const T&)>;

		template<typename T>
		using ValidatorFunc = std::function<bool(CityDraft::Drafts::Draft*, const T&)>;

		inline View(std::string_view name, CityDraft::Drafts::Draft* owner, GetterFunc<T> getter, SetterFunc<T> setter, ValidatorFunc<T> validator):
			TypedProperty<T>(name, owner),
			m_Getter(getter),
			m_Setter(setter),
			m_Validator(validator)
		{

		}

		inline const T& Get() const override
		{
			BOOST_ASSERT(m_Getter);
			return m_Getter(Property::m_Owner);
		}

		inline bool Set(const T& value) override
		{
			BOOST_ASSERT(m_Setter);

			if(m_Validator && !m_Validator(Property::m_Owner, value))
			{
				return false;
			}

			m_Setter(Property::m_Owner, value);
			return true;
		}

		inline bool IsReadable() const override
		{
			return m_Getter != nullptr;
		}

		inline virtual bool IsWritable() const override
		{
			return m_Setter != nullptr;
		}

	protected:
		GetterFunc<T> m_Getter;
		SetterFunc<T> m_Setter;
		ValidatorFunc<T> m_Validator;

		friend class CityDraft::Drafts::Draft;
	};
}