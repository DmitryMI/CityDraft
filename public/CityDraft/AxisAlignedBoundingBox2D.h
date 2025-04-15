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

		constexpr Vector2D GetMin() const
		{
			return Data.min_corner();
		}

		constexpr Vector2D GetMax() const
		{
			return Data.max_corner();
		}

		constexpr Vector2D GetSize() const
		{
			return GetMax() - GetMin();
		}

		constexpr Vector2D GetCenter() const
		{
			return (GetMax() + GetMin()) / 2;
		}
	};
}
