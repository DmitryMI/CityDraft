#pragma once

#include <CityDraft/LinearColor.h>

namespace CityDraft::UI::Colors
{
	class IColorsProvider
	{
	public:
		virtual LinearColorF GetSelectionBoxColor() = 0;
		virtual LinearColorF GetDraftPreSelectionBoxColor() = 0;
		virtual LinearColorF GetDraftScaleBoxColor() = 0;
		virtual LinearColorF GetDraftScaleKnobsColor() = 0;
		virtual LinearColorF GetDraftRotationCircleColor() = 0;
		virtual LinearColorF GetDraftRotationCircleHighlightedColor() = 0;
	};
}
