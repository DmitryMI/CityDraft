#pragma once

#include "IColorsProvider.h"

namespace CityDraft::UI::Colors
{
	class ConstColorsProvider : public IColorsProvider
	{
		QColor GetSelectionBoxColor() override { return QColor(66, 133, 244, 255); }
		QColor GetDraftPreSelectionBoxColor() override { return QColor(255, 191, 0, 153); }
		QColor GetDraftPostSelectionBoxColor() override { return QColor(30, 144, 255, 204); }
	};
}
