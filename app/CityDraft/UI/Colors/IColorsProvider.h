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
		virtual QColor GetDraftScaleKnobsColor() = 0;
		virtual QColor GetDraftRotationCircleColor() = 0;
		virtual QColor GetDraftRotationCircleHighlightedColor() = 0;
	};
}
