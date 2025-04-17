#pragma once

#include "Painter.h"
#include "include/core/SkCanvas.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Painter
	{
	public:
		virtual void Paint(SkCanvas* canvas) = 0;
	};
}
