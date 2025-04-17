#pragma once

#include "IKeyBindingProvider.h"

namespace CityDraft::Input
{
	class ConstKeyBindingProvider : public IKeyBindingProvider
	{
	public:
		inline Qt::MouseButton GetMouseSelectionButton() override { return Qt::MouseButton::LeftButton; };
		inline Qt::MouseButton GetMouseViewportPanningButton() override { return Qt::MouseButton::MiddleButton; };
		inline Qt::KeyboardModifier GetSelectionAdditiveModifier() override { return Qt::ShiftModifier; };
	};
}
