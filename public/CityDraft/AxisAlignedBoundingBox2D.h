#pragma once

#include <boost/geometry.hpp>
#include "Vector2D.h"
#include "Transform2D.h"

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

		void SetMin(const Vector2D& min)
		{
			Data = UnderlyingType(min, GetMax());
		}

		constexpr Vector2D GetMax() const
		{
			return Data.max_corner();
		}

		void SetMax(const Vector2D& max)
		{
			Data = UnderlyingType(GetMin(), max);
		}

		void SetMinMax(const Vector2D& min, const Vector2D& max)
		{
			Data = UnderlyingType(min, max);
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

		constexpr bool Contains(const Vector2D& point) const
		{
			return
				GetMin().GetX() <= point.GetX() && point.GetX() <= GetMax().GetX() &&
				GetMin().GetY() <= point.GetY() && point.GetY() <= GetMax().GetY();
		}

		inline void GetCircumcircle(Vector2D& center, double& radius) const
		{
			center = GetCenter();
			radius = 0.5f * sqrt(GetSize().GetX() * GetSize().GetX() + GetSize().GetY() * GetSize().GetY());
		}

		AxisAlignedBoundingBox2D& Transform(const Transform2D& transform)
		{
			std::array<Vector2D, 4> corners =
			{
				Vector2D{GetMin().GetX(), GetMin().GetY()},
				Vector2D{GetMax().GetX(), GetMin().GetY()},
				Vector2D{GetMax().GetX(), GetMax().GetY()},
				Vector2D{GetMin().GetX(), GetMax().GetY()}
			};

			double cosA = std::cos(transform.Rotation.Value);
			double sinA = std::sin(transform.Rotation.Value);

			Vector2D min = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
			Vector2D max = {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()};

			for(const Vector2D& corner : corners)
			{
				Vector2D scaledCorner = (corner - GetCenter()).ComponentMultiply(transform.Scale) + GetCenter();

				double rotatedX = scaledCorner.GetX() * cosA - scaledCorner.GetY() * sinA;
				double rotatedY = scaledCorner.GetX() * sinA + scaledCorner.GetY() * cosA;

				double worldX = rotatedX + transform.Translation.GetX();
				double worldY = rotatedY + transform.Translation.GetY();

				min = Vector2D{
					std::min(min.GetX(), worldX),
					std::min(min.GetY(), worldY)
				};
				max = Vector2D{
					std::max(max.GetX(), worldX),
					std::max(max.GetY(), worldY)
				};
			}

			SetMinMax(min, max);
			return *this;
		}

		AxisAlignedBoundingBox2D GetTransformed(const Transform2D& transform)
		{
			AxisAlignedBoundingBox2D copy = *this;
			return copy.Transform(transform);
		}
	};
}
