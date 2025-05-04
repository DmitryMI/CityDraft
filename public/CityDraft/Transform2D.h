#pragma once

#include <boost/geometry.hpp>
#include <boost/math/ccmath/abs.hpp>
#include "Vector2D.h"
#include "Angles.h"

namespace CityDraft
{
	/// <summary>
	/// Represents a transformation in 2D space
	/// </summary>
	struct Transform2D
	{
		Vector2D Translation{0, 0};
		Radians Rotation{0};
		Vector2D Scale{1, 1};

		void RotateAroundPoint(const Radians& angleRadians, const Vector2D& point)
		{
			Vector2D relativePos = Translation - point;
			relativePos.Rotate(angleRadians);
			Translation = relativePos + point;
			Rotation += angleRadians;
		}

		void ScaleRelativeToPoint(const Vector2D& scaleFactors, const Vector2D& point)
		{
			Vector2D relativePos = Translation - point;

			Vector2D scaledPos = Vector2D(
				relativePos.GetX() * scaleFactors.GetX(),
				relativePos.GetY() * scaleFactors.GetY()
			);

			Translation = scaledPos + point;

			Scale = Vector2D(
				Scale.GetX() * scaleFactors.GetX(),
				Scale.GetY() * scaleFactors.GetY()
			);
		}

		constexpr bool operator==(const Transform2D& b)
		{
			return
				(b.Translation - Translation).IsNearlyZero() &&
				boost::math::ccmath::abs(b.Rotation.Value - Rotation.Value) < 1E-9 &&
				(b.Scale - Scale).IsNearlyZero();
		}

		constexpr bool operator!=(const Transform2D& b)
		{
			return
				!(b.Translation - Translation).IsNearlyZero() ||
				boost::math::ccmath::abs(b.Rotation.Value - Rotation.Value) >= 1E-9 ||
				!(b.Scale - Scale).IsNearlyZero();
		}

		Vector2D ApplyTo(const Vector2D& point) const
		{
			Vector2D pointClone = point;
			pointClone.ComponentMultiply(Scale);

			pointClone.Rotate(Rotation);

			return pointClone += Translation;
		}
	};
}
