#include <gtest/gtest.h>
#include "CityDraft/LinearColor.h"

namespace CityDraft
{
	TEST(LinearColorTest, Construction)
	{
		constexpr auto color = "#2300ffFE"_frgba;
		constexpr uint8_t red = color.Red<uint8_t>();
		constexpr uint8_t green = color.Green<uint8_t>();
		constexpr uint8_t blue = color.Blue<uint8_t>();
		constexpr uint8_t alpha = color.Alpha<uint8_t>();
		ASSERT_EQ(red, 0x23);
		ASSERT_EQ(green, 0x00);
		ASSERT_EQ(blue, 0xff);
		ASSERT_EQ(alpha, 0xfe);
	}

}
