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
			BOOST_ASSERT(min.GetX() <= max.GetX() && min.GetY() <= max.GetY());
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

		constexpr bool operator ==(const AxisAlignedBoundingBox2D& b)
		{
			return (GetMin() - b.GetMin()).IsNearlyZero() && (GetMax() - b.GetMax()).IsNearlyZero();
		}

		constexpr bool operator !=(const AxisAlignedBoundingBox2D& b)
		{
			return !(GetMin() - b.GetMin()).IsNearlyZero() || !(GetMax() - b.GetMax()).IsNearlyZero();
		}

		inline void GetCircumcircle(Vector2D& center, double& radius)
		{
			center = GetCenter();
			radius = 0.5f * sqrt(GetSize().GetX() * GetSize().GetX() + GetSize().GetY() * GetSize().GetY());
		}
	};
}
