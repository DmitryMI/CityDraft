#pragma once

#include <QColor>
#include "CityDraft/LinearColor.h"
#include <include/core/SkColor.h>

namespace CityDraft::UI::Colors
{
	class Utils
	{
	public:

		constexpr static QColor ToQColor(const LinearColorF& color)
		{
			return QColor(color.Red<uint8_t>(), color.Green<uint8_t>(), color.Blue<uint8_t>(), color.Alpha<uint8_t>());
		}

		constexpr static SkColor ToSkColor(const LinearColorF& color)
		{
			return SkColorSetARGB(
				color.Alpha<uint8_t>(),
				color.Red<uint8_t>(),
				color.Green<uint8_t>(),
				color.Blue<uint8_t>()
			);
		}
	};
}