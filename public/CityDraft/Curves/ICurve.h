#pragma once

#include "CityDraft/Vector2D.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Transform2D.h"

namespace CityDraft::Curves
{
	class ICurve
	{
	public:
		virtual ~ICurve() = default;

		/// <summary>
		/// GetPoint point on a curve.
		/// </summary>
		/// <param name="t">Parameter in [0, 1]</param>
		/// <returns>Point</returns>
		virtual CityDraft::Vector2D GetPoint(double t) const = 0;

		/// <summary>
		/// GetPoint the tangent (not necessarily normalized)
		/// </summary>
		/// <param name="t">Parameter in [0, 1]</param>
		/// <returns>Tangent</returns>
		virtual CityDraft::Vector2D GetTangent(double t) const = 0;

		/// <summary>
		/// Normalized normal (perpendicular to tangent)
		/// </summary>
		/// <param name="t">Parameter in [0, 1]</param>
		/// <returns>Normal</returns>
		virtual CityDraft::Vector2D GetNormal(double t) const
		{
			CityDraft::Vector2D tan = GetTangent(t);
			return {-tan.GetY(),tan.GetX()};
		}

		/// <summary>
		/// Total length of the curve
		/// </summary>
		/// <returns>Curve Length</returns>
		virtual double GetLength(double t) const = 0;

		/// <summary>
		/// Calculates parameter T at distance along the Curve
		/// </summary>
		/// <param name="s">Distance along the Curve [0, GetLength()]</param>
		/// <returns>Parameter in [0, 1]</returns>
		virtual double GetParameterAtLength(double s) const = 0;

		/// <summary>
		/// Calculates a Curve Parameter, which represents a point on the Curve, closest to a given point.
		/// </summary>
		/// <param name="target">Target Point</param>
		/// <returns>Curve Parameter, associated with a point, closest to Target</returns>
		virtual double GetClosestParameter(const CityDraft::Vector2D& target) const = 0;

		/// <summary>
		/// Transforms the curve
		/// </summary>
		/// <param name="transform">Transform</param>
		virtual void Transform(const Transform2D& transform) = 0;
	};
}