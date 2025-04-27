#pragma once

#include "ICurve.h"

namespace CityDraft::Curves
{
	class LineSegment: public ICurve
	{
	public:
		LineSegment(const CityDraft::Vector2D& start, const CityDraft::Vector2D& end);
		LineSegment();

		void SetStart(const CityDraft::Vector2D& start);
		void SetEnd(const CityDraft::Vector2D& end);
		void SetPoints(const CityDraft::Vector2D& start, const CityDraft::Vector2D& end);

		const CityDraft::Vector2D& GetStart() const;
		const CityDraft::Vector2D& GetEnd() const;

		// ICurve
		CityDraft::Vector2D GetPoint(double t) const override;

		CityDraft::Vector2D GetTangent(double t) const override;

		CityDraft::Vector2D GetNormal(double t) const override
		{
			CityDraft::Vector2D tan = GetTangent(t);
			return {-tan.GetY(), tan.GetX()};
		}

		double GetLength(double t) const override;

		double GetParameterAtLength(double s) const override;

		double GetClosestParameter(const CityDraft::Vector2D& target) const override;

		void Transform(const Transform2D& transform) override;

		AxisAlignedBoundingBox2D GetBoundingBox() const override;

	private:
		CityDraft::Vector2D m_Start;
		CityDraft::Vector2D m_End;
		double m_Length;
	};
}
