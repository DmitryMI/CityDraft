#pragma once

#include "Property.h"
#include <cstdint>

namespace CityDraft::Drafts::Properties
{
	template<typename T>
	class View : public Property
	{
	public:
		inline View(std::string_view name, CityDraft::Drafts::Draft* owner, GetterFunc<T> getter, SetterFunc<T> setter, ValidatorFunc<T> validator):
			Property(name, owner),
			m_Getter(getter),
			m_Setter(setter),
			m_Validator(validator)
		{

		}

		inline const T& Get() const
		{
			BOOST_ASSERT(m_Getter);
			return m_Getter(m_Owner);
		}

		inline bool Set(const T& value)
		{
			BOOST_ASSERT(m_Setter);

			if(m_Validator && !m_Validator(m_Owner, value))
			{
				return false;
			}

			m_Setter(m_Owner, value);
			return true;
		}

		inline std::any GetAny() const override
		{
			return Get();
		}

		inline bool SetAny(std::any value) override
		{
			if(value.type() == typeid(T))
			{
				return Set(std::any_cast<T>(value));
			}
			
			return false;
		}

	protected:
		GetterFunc<T> m_Getter;
		SetterFunc<T> m_Setter;
		ValidatorFunc<T> m_Validator;
	};
}