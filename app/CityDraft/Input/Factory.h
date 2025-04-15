#pragma once

#include "ConstKeyBindingProvider.h"
#include <memory>

namespace CityDraft::Input
{
	class Factory
	{
	public:
		static std::shared_ptr<IKeyBindingProvider> CreateKeyBindingProvider();
	};
}
