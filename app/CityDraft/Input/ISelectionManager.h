#pragma once

#include <CityDraft/Drafts/Draft.h>
#include <set>
#include <memory>

namespace CityDraft::Input
{
	class ISelectionManager
	{
	public:
		virtual const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& GetSelectedDrafts() const = 0;
		virtual void ClearSelectedDrafts() = 0;
		virtual void AddDraftsToSelection(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>&) = 0;
	};
}
