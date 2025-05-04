#include "CityDraft/Drafts/Properties/Property.h"

namespace CityDraft::Drafts::Properties
{
	Map FindCommonProperties(const std::vector<Set>& sets)
	{
		if(sets.size() == 0)
		{
			return {};
		}

		Map result;

		std::set<std::string_view> names;

		for(const auto& prop : sets[0])
		{
			names.insert(prop->GetName());
		}

		for(size_t i = 1; i < sets.size(); i++)
		{
			for(const auto& prop : sets[i])
			{
				std::string_view name = prop->GetName();
				if(!names.contains(name))
				{
					names.erase(name);
				}
			}

			std::set<std::string_view> namesCopy = names;
			for(const auto& name : namesCopy)
			{
				if(!std::any_of(sets[i].begin(), sets[i].end(), [name](const auto& prop){return prop->GetName() == name;}))
				{
					names.erase(name);
				}
			}
		}

		for(const auto& name : names)
		{
			for(const auto& set : sets)
			{
				for(const auto& prop : set)
				{
					if(prop->GetName() == name)
					{
						result[name].push_back(prop);
					}
				}
			}
		}

		return result;
	}
}