#include "Utils.h"

namespace CityDraft::UI::Colors
{
	QColor Utils::ToQColor(const LinearColorF& color)
	{
		return QColor(color.Red<uint8_t>(), color.Green<uint8_t>(), color.Blue<uint8_t>(), color.Alpha<uint8_t>());
	}


	SkColor CityDraft::UI::Colors::Utils::ToSkColor(const LinearColorF& color)
	{
		return SkColorSetARGB(
				color.Alpha<uint8_t>(),
				color.Red<uint8_t>(),
				color.Green<uint8_t>(),
				color.Blue<uint8_t>()
		);
	}
}