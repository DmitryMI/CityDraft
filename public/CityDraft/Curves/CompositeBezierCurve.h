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
			CityDraft::Vector2D Position{0,0};
			CityDraft::Vector2D OutgoingHandle{0, 0}; // Relative to Position
			CityDraft::Vector2D IncomingHandle{0, 0}; // Relative to Position
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

		void GetNearestAnchorIndices(double t, std::vector<size_t>& outAnchorIndices) const;

		inline void ChangeAnchor(size_t index, const Anchor& anchor)
		{
			BOOST_ASSERT(index < m_Anchors.size());
			m_Anchors[index] = anchor;
			RebuildArcLengthTable();
		}

		inline void AddAnchor(const Anchor& anchor, bool toEnd = true)
		{
			if(toEnd)
			{
				m_Anchors.push_back(anchor);
			}
			else
			{
				m_Anchors.insert(m_Anchors.begin(), anchor);
			}
			RebuildArcLengthTable();
		}

		inline void InsertAnchor(size_t index, const Anchor& anchor)
		{
			BOOST_ASSERT(index < m_Anchors.size());
			m_Anchors.insert(m_Anchors.begin() + index, anchor);
			RebuildArcLengthTable();
		}

		size_t InsertAnchor(double t);

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
