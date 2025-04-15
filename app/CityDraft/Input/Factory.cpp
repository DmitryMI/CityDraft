#include "Factory.h"
#include "ConstKeyBindingProvider.h"

namespace CityDraft::Input
{
	std::shared_ptr<IKeyBindingProvider> Factory::CreateKeyBindingProvider()
	{
		return std::make_shared<ConstKeyBindingProvider>();
	}
}
