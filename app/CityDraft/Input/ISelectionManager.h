#pragma once

#include <CityDraft/Drafts/Draft.h>
#include <set>
#include <memory>
#include <boost/signals2.hpp>
#include <vector>

namespace CityDraft::Input
{
	class ISelectionManager
	{
	public:
		using DraftsSelectedSignal = boost::signals2::signal<void(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>&)>;
		using DraftsDeselectedSignal = boost::signals2::signal<void(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>&)>;

		virtual const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& GetSelectedDrafts() const = 0;
		virtual void ClearSelectedDrafts() = 0;
		virtual void AddDraftsToSelection(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>&) = 0;
		virtual void RemoveDraftsFromSelection(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>&) = 0;
		virtual boost::signals2::connection ConnectToDraftsSelected(DraftsSelectedSignal::slot_type slot) = 0;
		virtual boost::signals2::connection ConnectToDraftsDeselected(DraftsDeselectedSignal::slot_type slot) = 0;
	};
}
