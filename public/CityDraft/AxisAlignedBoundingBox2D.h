#pragma once

#include <boost/geometry.hpp>
#include "Vector2D.h"

namespace CityDraft
{

	struct AxisAlignedBoundingBox2D
	{
		using UnderlyingType = boost::geometry::model::box<Vector2D::UnderlyingType>;

		UnderlyingType Data;

		constexpr AxisAlignedBoundingBox2D(const Vector2D& min, const Vector2D& max) : Data{ min, max }
		{

		}

		constexpr AxisAlignedBoundingBox2D(const UnderlyingType& box) : Data{ box }
		{

		}

		constexpr operator UnderlyingType() const
		{
			return Data;
		}
	};
}
