#pragma once

#include "IColorsProvider.h"

namespace CityDraft::UI::Colors
{
	class ConstColorsProvider : public IColorsProvider
	{
		LinearColorF GetSelectionBoxColor() override { return "#4285F4FF"_frgba; }
		LinearColorF GetDraftPreSelectionBoxColor() override { return "#FFBF0099"_frgba; }
		LinearColorF GetDraftScaleBoxColor() override { return "#DCEBFFA0"_frgba; }
		LinearColorF GetDraftScaleKnobsColor() override { return "#DCEBFFFF"_frgba; }
		LinearColorF GetDraftRotationCircleColor() override { return "#FF8A65A0"_frgba; };
		LinearColorF GetDraftRotationCircleHighlightedColor() override { return "#FFA883FA"_frgba; };
	};
}
