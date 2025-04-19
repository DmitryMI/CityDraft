#pragma once

#include <QColor>

namespace CityDraft::UI::Colors
{
	class IColorsProvider
	{
	public:
		virtual QColor GetSelectionBoxColor() = 0;
		virtual QColor GetDraftPreSelectionBoxColor() = 0;
		virtual QColor GetDraftScaleBoxColor() = 0;
		virtual QColor GetDraftRotationCircleColor() = 0;
	};
}
