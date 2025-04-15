#pragma once

#include <QMouseEvent>

namespace Input
{
	class IKeyBindingProvider
	{
	public:
		virtual Qt::MouseButton GetMouseSelectionButton() = 0;
		virtual Qt::MouseButton GetMouseViewportPanningButton() = 0;
	};
}
