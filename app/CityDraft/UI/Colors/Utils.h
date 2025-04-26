#pragma once

#include <QColor>
#include "CityDraft/LinearColor.h"
#include <include/core/SkColor.h>

namespace CityDraft::UI::Colors
{
	class Utils
	{
	public:

		static QColor ToQColor(const LinearColorF& color);
		static SkColor ToSkColor(const LinearColorF& color);
	};
}