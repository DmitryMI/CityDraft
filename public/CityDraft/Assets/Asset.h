#pragma once

#include "CityDraft/DraftObject.h"


namespace CityDraft::Assets
{
	class Asset
	{
	public:
		virtual DraftObject* CreateDraftObject() = 0;
	};
}
