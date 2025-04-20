#pragma once

#include "IColorsProvider.h"
#include "ConstColorsProvider.h"
#include <memory>

namespace CityDraft::UI::Colors
{
	class Factory
	{
	public:
		static std::shared_ptr<IColorsProvider> CreateColorsProviderProvider();
	};
}
