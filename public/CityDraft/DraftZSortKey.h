#pragma once

namespace CityDraft
{
	template<typename T>
	struct DraftZSortKey
	{
		int64_t LayerZ;
		int64_t DraftZ;
		T Draft;

		DraftZSortKey(T draft)
		{
			LayerZ = draft->GetLayer()->GetZOrder();
			DraftZ = draft->GetZOrder();
			Draft = draft;
		}

		bool operator<(const DraftZSortKey& other) const
		{
			if(LayerZ != other.LayerZ)
				return LayerZ < other.LayerZ;
			if(DraftZ != other.DraftZ)
				return DraftZ < other.DraftZ;
			return Draft < other.Draft;
		}
	};

}