#pragma once

#include "ICurve.h"
#include <vector>
#include <algorithm>
#include <boost/assert.hpp>

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

		void AddAnchor(const Anchor& anchor)
		{
			m_Anchors.push_back(anchor);
			RebuildArcLengthTable();
		}

		void ClearAnchors()
		{
			m_Anchors.clear();
			m_ArcLengthTable.clear();
		}

		CityDraft::Vector2D GetPoint(double t) const override
		{
			if(m_Anchors.size() < 2) return {};

			double totalSegments = m_Anchors.size() - 1;
			double scaledT = std::clamp(t,0.0,1.0) * totalSegments;

			size_t segmentIndex = std::min(static_cast<size_t>(scaledT),m_Anchors.size() - 2);
			double localT = scaledT - segmentIndex;

			return EvaluateSegment(segmentIndex,localT);
		}

		CityDraft::Vector2D GetTangent(double t) const override
		{
			const double delta = 1e-5;
			CityDraft::Vector2D a = GetPoint(std::max(0.0,t - delta));
			CityDraft::Vector2D b = GetPoint(std::min(1.0,t + delta));
			return (b - a);
		}

		double GetLength() const override
		{
			if(m_ArcLengthTable.empty()) return 0.0;
			return m_ArcLengthTable.back().length;
		}

		double GetLength(double t) const override
		{
			if(t <= 0.0) return 0.0;
			if(t >= 1.0) return GetLength(); // full length for t = 1

			// For any t in [0, 1), calculate the length up to that t
			double totalLength = 0.0;
			const int samples = 100;
			Vector2D prev = Evaluate(0.0);

			for(int i = 1; i <= samples; ++i)
			{
				double sampledT = static_cast<double>(i) / samples;
				if(sampledT > t) break;  // Stop once we reach 't'
				Vector2D current = GetPoint(sampledT);
				totalLength += (current - prev).Length();
				prev = current;
			}

			return totalLength;
		}

		double GetParameterAtLength(double s) const override
		{
			if(m_ArcLengthTable.empty()) return 0.0;
			if(s <= 0.0) return 0.0;
			if(s >= GetLength()) return 1.0;

			auto it = std::lower_bound(
				m_ArcLengthTable.begin(),m_ArcLengthTable.end(),s,
				[](const ArcEntry& entry,double value) {
				return entry.length < value;
			});

			if(it == m_ArcLengthTable.begin()) return it->t;

			const ArcEntry& next = *it;
			const ArcEntry& prev = *(it - 1);

			double dt = next.t - prev.t;
			double ds = next.length - prev.length;
			double alpha = (s - prev.length) / ds;

			return prev.t + alpha * dt;
		}

		const std::vector<Anchor>& GetAnchors() const
		{
			return m_Anchors;
		}

		double GetClosestParameter(const CityDraft::Vector2D& target) const
		{
			constexpr int samples = 100;
			double bestT = 0.0;
			double bestDistSquared = std::numeric_limits<double>::max();

			for(int i = 0; i <= samples; ++i)
			{
				double t = static_cast<double>(i) / samples;
				CityDraft::Vector2D pt = GetPoint(t);
				double distSq = (pt - target).LengthSquared();

				if(distSq < bestDistSquared)
				{
					bestDistSquared = distSq;
					bestT = t;
				}
			}

			// Optional: refine with local Newton-Raphson or binary search here

			return bestT;
		}

	private:
		struct ArcEntry
		{
			double t;      // Global parameter t in [0, 1]
			double length; // Arc length from t=0 to this point
		};

		std::vector<Anchor> m_Anchors;
		std::vector<ArcEntry> m_ArcLengthTable;

		CityDraft::Vector2D EvaluateSegment(size_t i,double t) const
		{
			BOOST_ASSERT(i < m_Anchors.size() - 1);

			const Anchor& a0 = m_Anchors[i];
			const Anchor& a1 = m_Anchors[i + 1];

			CityDraft::Vector2D p0 = a0.Position;
			CityDraft::Vector2D p1 = a0.Position + a0.OutgoingHandle;
			CityDraft::Vector2D p2 = a1.Position + a1.IncomingHandle;
			CityDraft::Vector2D p3 = a1.Position;

			double u = 1.0 - t;
			return u * u * u * p0 +
				3.0 * u * u * t * p1 +
				3.0 * u * t * t * p2 +
				t * t * t * p3;
		}

		void RebuildArcLengthTable()
		{
			m_ArcLengthTable.clear();
			constexpr int samples = 100;

			CityDraft::Vector2D prev = GetPoint(0.0);
			double totalLength = 0.0;
			m_ArcLengthTable.push_back({0.0,0.0});

			for(int i = 1; i <= samples; ++i)
			{
				double t = static_cast<double>(i) / samples;
				CityDraft::Vector2D pt = GetPoint(t);
				totalLength += (pt - prev).Length();
				m_ArcLengthTable.push_back({t,totalLength});
				prev = pt;
			}
		}
	};
}
