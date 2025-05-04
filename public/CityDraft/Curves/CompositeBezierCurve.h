#pragma once

#include "ICurve.h"
#include <vector>
#include <algorithm>
#include <boost/assert.hpp>
#include "CityDraft/Serialization/IArchive.h"
#include "Factory.h"

namespace CityDraft::Curves
{
	class CompositeBezierCurve: public ICurve
	{
	public:
		struct Anchor
		{
			CityDraft::Vector2D Position;
			CityDraft::Vector2D OutgoingHandle; // Relative to Position
			CityDraft::Vector2D IncomingHandle; // Relative to Position
		};

		CompositeBezierCurve() = default;

		inline CompositeBezierCurve(const std::vector<Anchor>& anchors):
			m_Anchors(anchors)
		{
			
		}

		inline const std::vector<Anchor>& GetAnchors() const
		{
			return m_Anchors;
		}

		inline void AddAnchor(const Anchor& anchor)
		{
			m_Anchors.push_back(anchor);
			RebuildArcLengthTable();
		}

		inline void ClearAnchors()
		{
			m_Anchors.clear();
			m_ArcLengthTable.clear();
		}

		CityDraft::Vector2D GetPoint(double t) const override;

		CityDraft::Vector2D GetTangent(double t) const override;

		double GetLength(double t) const override;

		double GetParameterAtLength(double s) const override;

		double GetClosestParameter(const CityDraft::Vector2D& target) const override;

		void Transform(const Transform2D& transform) override;

		AxisAlignedBoundingBox2D GetBoundingBox() const override;

		void Serialize(CityDraft::Serialization::IOutputArchive& archive) const override;

		void Deserialize(CityDraft::Serialization::IInputArchive& archive) override;

	private:
		struct ArcEntry
		{
			double t;      // Global parameter t in [0, 1]
			double length; // Arc length from t=0 to this point
		};

		std::vector<Anchor> m_Anchors;
		std::vector<ArcEntry> m_ArcLengthTable;

		CityDraft::Vector2D EvaluateSegment(size_t i, double t) const;

		void RebuildArcLengthTable();

		void FindBezierExtrema(double p0, double p1, double p2, double p3, std::vector<double>& ts) const;

		REGISTER_TYPE(CompositeBezierCurve);
	};
}
