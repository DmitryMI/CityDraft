#pragma once

#include "IColorsProvider.h"

namespace CityDraft::UI::Colors
{
	class ConstColorsProvider : public IColorsProvider
	{
		QColor GetSelectionBoxColor() override { return QColor(66, 133, 244, 255); }
		QColor GetDraftPreSelectionBoxColor() override { return QColor(255, 191, 0, 153); }
		QColor GetDraftScaleBoxColor() override { return QColor(220, 235, 255, 160); }
		QColor GetDraftRotationCircleColor() override { return QColor(255, 138, 101, 160); };
		QColor GetDraftRotationCircleHighlightedColor() override { return QColor(255, 168, 131, 250); };
	};
}
