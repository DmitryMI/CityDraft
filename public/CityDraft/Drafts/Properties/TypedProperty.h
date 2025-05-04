#pragma once

#include "Property.h"

namespace CityDraft::Drafts::Properties
{
	template <typename T>
	class TypedProperty : public Property
	{
	public:
		using Property::Property;

		virtual const T& Get() const = 0;
		virtual bool Set(const T& value) = 0;

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

		friend class CityDraft::Drafts::Draft;
	};

	template<typename T>
	using TypedPropertyGroup = std::vector<std::shared_ptr<TypedProperty<T>>>;
}