#include "CityDraft/Curves/LineSegment.h"
#include <cmath>

namespace CityDraft::Curves
{
	LineSegment::LineSegment(const CityDraft::Vector2D& start, const CityDraft::Vector2D& end):
		m_Start(start),
		m_End(end),
		m_Length((start - end).GetSize())
	{

	}

	LineSegment::LineSegment():
		m_Start(),
		m_End(),
		m_Length(0)
	{

	}

	void LineSegment::SetStart(const CityDraft::Vector2D& start)
	{
		SetPoints(start, GetEnd());
	}

	void LineSegment::SetEnd(const CityDraft::Vector2D& end)
	{
		SetPoints(GetStart(), end);
	}

	void LineSegment::SetPoints(const CityDraft::Vector2D& start, const CityDraft::Vector2D& end)
	{
		m_Start = start;
		m_End = end;
		m_Length = (m_Start - m_End).GetSize();
	}

	const CityDraft::Vector2D& LineSegment::GetStart() const
	{
		return m_Start;
	}

	const CityDraft::Vector2D& LineSegment::GetEnd() const
	{
		return m_End;
	}

	CityDraft::Vector2D LineSegment::GetPoint(double t) const
	{
		BOOST_ASSERT(0 <= t && t <= 1);
		return CityDraft::Vector2D::Lerp(m_Start, m_End, t);
	}

	CityDraft::Vector2D LineSegment::GetTangent(double t) const
	{
		return {m_Start - m_End};
	}

	double LineSegment::GetLength(double t) const
	{
		return m_Length * t;
	}

	double LineSegment::GetParameterAtLength(double s) const
	{
		return s / m_Length;
	}

	double LineSegment::GetClosestParameter(const CityDraft::Vector2D& target) const
	{
		Vector2D segmentDelta = m_Start - m_End;
		Vector2D targetStartDelta = target - m_Start;
		// double lengthSquared = Vector2D::Dot(segmentDelta, segmentDelta);
		double lengthSquared = m_Length * m_Length;
		if(lengthSquared == 0)
		{
			return 0;
		}

		double t = Vector2D::Dot(targetStartDelta, segmentDelta) / lengthSquared;
		return std::clamp(t, 0.0, 1.0);
	}

	void LineSegment::Transform(const Transform2D& transform)
	{
		std::array<Vector2D, 2> points{m_Start, m_End};
		for(Vector2D& point : points)
		{
			point.ComponentMultiply(transform.Scale);
			point.Rotate(transform.Rotation);
			point += transform.Translation;
		}

		m_Start = points[0];
		m_End = points[1];
		m_Length = (m_End - m_Start).GetSize();
	}

	AxisAlignedBoundingBox2D LineSegment::GetBoundingBox() const
	{
		double minX = fmin(m_Start.GetX(), m_End.GetX());
		double minY = fmin(m_Start.GetY(), m_End.GetY());
		double maxX = fmax(m_Start.GetX(), m_End.GetX());
		double maxY = fmax(m_Start.GetY(), m_End.GetY());

		return {{minX, minY}, {maxX, maxY}};
	}
}