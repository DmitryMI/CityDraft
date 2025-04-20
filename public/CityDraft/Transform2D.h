#pragma once

#include <boost/geometry.hpp>
#include "Vector2D.h"

namespace CityDraft
{
	/// <summary>
	/// Represents a transformation in 2D space
	/// </summary>
	struct Transform2D
	{
		Vector2D Translation{0, 0};
		double Rotation{0};
		Vector2D Scale{1, 1};

		void RotateAroundPoint(double angleRadians, const Vector2D& point)
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
	};
}
