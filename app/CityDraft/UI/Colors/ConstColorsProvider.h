#pragma once

#include "IColorsProvider.h"

namespace CityDraft::UI::Colors
{
	class ConstColorsProvider: public IColorsProvider
	{
		LinearColorF GetSelectionBoxColor() override
		{
			return "#4285F4FF"_frgba;
		}
		LinearColorF GetDraftPreSelectionBoxColor() override
		{
			return "#FFBF0099"_frgba;
		}
		LinearColorF GetDraftScaleBoxColor() override
		{
			return "#DCEBFFA0"_frgba;
		}
		LinearColorF GetDraftScaleKnobsColor() override
		{
			return "#DCEBFFFF"_frgba;
		}
		LinearColorF GetDraftRotationCircleColor() override
		{
			return "#FF8A65A0"_frgba;
		};
		LinearColorF GetDraftRotationCircleHighlightedColor() override
		{
			return "#FFA883FA"_frgba;
		};

		LinearColorF GetCurveAnchorPointOutlineColor() override
		{
			return "#000000FF"_frgba;
		}

		LinearColorF GetCurveAnchorPointFillColor() override
		{
			return "#F44336FF"_frgba;
		}

		LinearColorF GetCurveHandlePointColor() override
		{
			return "#03A9F4FF"_frgba;
		}

		LinearColorF GetCurveHandleLineColor() override
		{
			return "#90CAF9FF"_frgba;
		}
	};
}
