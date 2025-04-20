#include "Factory.h"

namespace CityDraft::UI::Colors
{

	std::shared_ptr<IColorsProvider> CityDraft::UI::Colors::Factory::CreateColorsProviderProvider()
	{
		return std::make_shared<ConstColorsProvider>();
	}

}
