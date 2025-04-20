#pragma once

#include <QMouseEvent>

namespace CityDraft::Input
{
	class IKeyBindingProvider
	{
	public:
		virtual Qt::MouseButton GetMouseSelectionButton() = 0;
		virtual Qt::MouseButton GetMouseViewportPanningButton() = 0;
		virtual Qt::KeyboardModifier GetSelectionAdditiveModifier() = 0;
		virtual Qt::KeyboardModifier GetScaleFromCenterModifier() = 0;
	};
}
