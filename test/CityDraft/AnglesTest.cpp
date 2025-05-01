#include <gtest/gtest.h>
#include "CityDraft/Angles.h"

namespace CityDraft
{
	TEST(AnglesTest, RadiansLiteral)
	{
		constexpr auto angle = 1.2_rad;
		ASSERT_NEAR(angle.Value, 1.2, 0.001);
	}

	TEST(AnglesTest, DegreesLiteral)
	{
		constexpr auto angle = 30.5_deg;
		ASSERT_NEAR(angle.Value, 30.5, 0.001);
	}

	TEST(AnglesTest, RadiansToDegrees)
	{
		constexpr auto rad = 0.785398_rad;
		constexpr Degrees deg(rad);
		ASSERT_NEAR(deg.Value, 45, 0.01);
	}


	TEST(AnglesTest, DegreesToRadians)
	{
		constexpr auto deg = 30_deg;
		constexpr Radians rad(deg);
		ASSERT_NEAR(rad.Value, 0.523599, 0.01);
	}
}
