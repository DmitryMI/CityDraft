#include "CityDraft/Curves/CompositeBezierCurve.h"

namespace CityDraft::Curves
{
	void CompositeBezierCurve::GetNearestAnchorIndices(double t, std::vector<size_t>& outAnchorIndices) const
	{
		if(m_Anchors.size() < 2)
		{
			return;
		}

		double totalSegments = m_Anchors.size() - 1;
		double scaledT = std::clamp(t, 0.0, 1.0) * totalSegments;

		size_t segmentIndex = std::min(static_cast<size_t>(scaledT), m_Anchors.size() - 2);
		outAnchorIndices.push_back(segmentIndex);
		outAnchorIndices.push_back(segmentIndex + 1);
	}

	size_t CompositeBezierCurve::InsertAnchor(double t)
	{
		double totalSegments = m_Anchors.size() - 1;
		double scaledT = std::clamp(t, 0.0, 1.0) * totalSegments;

		size_t indexA = std::min(static_cast<size_t>(scaledT), m_Anchors.size() - 2);
		double localT = scaledT - indexA;
		size_t indexB = indexA + 1;

		auto& anchorA = m_Anchors[indexA];
		auto& anchorB = m_Anchors[indexB];
		Vector2D p0 = anchorA.Position;
		Vector2D p1 = p0 + anchorA.OutgoingHandle;
		Vector2D p2 = anchorB.Position + anchorB.IncomingHandle;
		Vector2D p3 = anchorB.Position;

		Vector2D q0 = Vector2D::Lerp(p0, p1, localT);
		Vector2D q1 = Vector2D::Lerp(p1, p2, localT);
		Vector2D q2 = Vector2D::Lerp(p2, p3, localT);

		Vector2D r0 = Vector2D::Lerp(q0, q1, localT);
		Vector2D r1 = Vector2D::Lerp(q1, q2, localT);

		Vector2D s = Vector2D::Lerp(r0, r1, localT);

		Vector2D tangent = (r1 - r0).GetNormalized();

		// Step 4: Approximate handle length
		// Option A: Average distance of R0 and R1 from S
		double len1 = (r0 - s).GetSize();
		double len2 = (r1 - s).GetSize();
		double avgLen = 0.5f * (len1 + len2);

		// Step 5: Construct new anchor with symmetric handles
		Anchor newAnchor;
		newAnchor.Position = s;
		newAnchor.IncomingHandle = -tangent * avgLen;
		newAnchor.OutgoingHandle = tangent * avgLen;

		m_Anchors.insert(m_Anchors.begin() + indexB, newAnchor);

		RebuildArcLengthTable();
		return indexB;
	}

	CityDraft::Vector2D CompositeBezierCurve::GetPoint(double t) const
	{
		if(m_Anchors.size() < 2) return {};

		double totalSegments = m_Anchors.size() - 1;
		double scaledT = std::clamp(t, 0.0, 1.0) * totalSegments;

		size_t segmentIndex = std::min(static_cast<size_t>(scaledT), m_Anchors.size() - 2);
		double localT = scaledT - segmentIndex;

		return EvaluateSegment(segmentIndex, localT);
	}

	CityDraft::Vector2D CompositeBezierCurve::GetTangent(double t) const
	{
		const double delta = 1e-5;
		CityDraft::Vector2D a = GetPoint(std::max(0.0, t - delta));
		CityDraft::Vector2D b = GetPoint(std::min(1.0, t + delta));
		return (b - a);
	}

	double CompositeBezierCurve::GetLength(double t) const
	{
		if(t <= 0.0) return 0.0;
		if(t >= 1.0) return GetLength(1.0); // full length for t = 1

		// For any t in [0, 1), calculate the length up to that t
		double totalLength = 0.0;
		const int samples = 100;
		Vector2D prev = GetPoint(0.0);

		for(int i = 1; i <= samples; ++i)
		{
			double sampledT = static_cast<double>(i) / samples;
			if(sampledT > t) break;  // Stop once we reach 't'
			Vector2D current = GetPoint(sampledT);
			totalLength += (current - prev).GetSize();
			prev = current;
		}

		return totalLength;
	}

	double CompositeBezierCurve::GetParameterAtLength(double s) const
	{
		if(m_ArcLengthTable.empty()) return 0.0;
		if(s <= 0.0) return 0.0;
		if(s >= GetLength(1.0)) return 1.0;

		auto it = std::lower_bound(
			m_ArcLengthTable.begin(), m_ArcLengthTable.end(), s,
			[](const ArcEntry& entry, double value) {
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

	double CompositeBezierCurve::GetClosestParameter(const CityDraft::Vector2D& target) const
	{
		constexpr int samples = 100;
		double bestT = 0.0;
		double bestDistSquared = std::numeric_limits<double>::max();

		for(int i = 0; i <= samples; ++i)
		{
			double t = static_cast<double>(i) / samples;
			CityDraft::Vector2D pt = GetPoint(t);
			double distSq = (pt - target).GetSizeSquared();

			if(distSq < bestDistSquared)
			{
				bestDistSquared = distSq;
				bestT = t;
			}
		}

		// Optional: refine with local Newton-Raphson or binary search here

		return bestT;
	}

	void CompositeBezierCurve::Serialize(CityDraft::Serialization::IOutputArchive& archive) const
	{
		archive << m_Anchors.size();
		for(const auto& anchor : m_Anchors)
		{
			archive << anchor.Position << anchor.OutgoingHandle << anchor.IncomingHandle;
		}
	}

	void CompositeBezierCurve::Deserialize(CityDraft::Serialization::IInputArchive& archive)
	{
		size_t anchorsNum;
		archive >> anchorsNum;
		for(size_t i = 0; i < anchorsNum; i++)
		{
			Anchor anchor;
			archive >> anchor.Position;
			archive >> anchor.OutgoingHandle;
			archive >> anchor.IncomingHandle;
			m_Anchors.push_back(anchor);
		}

		RebuildArcLengthTable();
	}

	CityDraft::Vector2D CompositeBezierCurve::EvaluateSegment(size_t i, double t) const
	{
		BOOST_ASSERT(i < m_Anchors.size() - 1);

		const Anchor& a0 = m_Anchors[i];
		const Anchor& a1 = m_Anchors[i + 1];

		CityDraft::Vector2D p0 = a0.Position;
		CityDraft::Vector2D p1 = a0.Position + a0.OutgoingHandle;
		CityDraft::Vector2D p2 = a1.Position + a1.IncomingHandle;
		CityDraft::Vector2D p3 = a1.Position;

		double u = 1.0 - t;
		return p0 * u * u * u +
			p1 * 3.0 * u * u * t +
			p2 * 3.0 * u * t * t +
			p3 * t * t * t;
	}

	void CompositeBezierCurve::RebuildArcLengthTable()
	{
		m_ArcLengthTable.clear();
		constexpr int samples = 100;

		CityDraft::Vector2D prev = GetPoint(0.0);
		double totalLength = 0.0;
		m_ArcLengthTable.push_back({0.0, 0.0});

		for(int i = 1; i <= samples; ++i)
		{
			double t = static_cast<double>(i) / samples;
			CityDraft::Vector2D pt = GetPoint(t);
			totalLength += (pt - prev).GetSize();
			m_ArcLengthTable.push_back({t, totalLength});
			prev = pt;
		}
	}

	void CompositeBezierCurve::Transform(const Transform2D& transform)
	{
		Transform2D untranslated{{0, 0}, transform.Rotation, transform.Scale};
		for(auto& anchor : m_Anchors)
		{
			anchor.Position = transform.ApplyTo(anchor.Position);

			anchor.OutgoingHandle = untranslated.ApplyTo(anchor.OutgoingHandle);
			anchor.IncomingHandle = untranslated.ApplyTo(anchor.IncomingHandle);
		}
	}

	void CompositeBezierCurve::FindBezierExtrema(double p0, double p1, double p2, double p3, std::vector<double>& ts) const
	{
		double a = -p0 + 3 * p1 - 3 * p2 + p3;
		double b = 2 * p0 - 4 * p1 + 2 * p2;
		double c = -p0 + p1;

		if(std::abs(a) < 1e-6)
		{
			if(std::abs(b) < 1e-6) return;
			double t = -c / b;
			if(t > 0 && t < 1) ts.push_back(t);
			return;
		}

		double discriminant = b * b - 4 * a * c;
		if(discriminant < 0) return;

		double sqrtD = std::sqrt(discriminant);
		double t1 = (-b + sqrtD) / (2 * a);
		double t2 = (-b - sqrtD) / (2 * a);

		if(t1 > 0 && t1 < 1) ts.push_back(t1);
		if(t2 > 0 && t2 < 1) ts.push_back(t2);
	}

	AxisAlignedBoundingBox2D CompositeBezierCurve::GetBoundingBox() const
	{
		constexpr Vector2D boxMin{std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
		constexpr Vector2D boxMax{std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()};
		AxisAlignedBoundingBox2D bounds{boxMin, boxMax};

		for(size_t i = 0; i < m_Anchors.size() - 1; ++i)
		{
			const auto& prev = m_Anchors[i];
			const auto& curr = m_Anchors[i + 1];

			Vector2D p0 = prev.Position;
			Vector2D p1 = prev.Position + prev.OutgoingHandle;
			Vector2D p2 = curr.Position + curr.IncomingHandle;
			Vector2D p3 = curr.Position;

			// Include endpoints and control points
			bounds.ExpandToInclude(p0);
			bounds.ExpandToInclude(p1);
			bounds.ExpandToInclude(p2);
			bounds.ExpandToInclude(p3);

			// Find t where dx/dt or dy/dt = 0
			std::vector<double> tsX, tsY;
			FindBezierExtrema(p0.GetX(), p1.GetX(), p2.GetX(), p3.GetX(), tsX);
			FindBezierExtrema(p0.GetY(), p1.GetY(), p2.GetY(), p3.GetY(), tsY);

			for(float t : tsX)
			{
				Vector2D pt = EvaluateSegment(i, t);
				bounds.ExpandToInclude(pt);
			}
			for(float t : tsY)
			{
				Vector2D pt = EvaluateSegment(i, t);
				bounds.ExpandToInclude(pt);
			}
		}

		BOOST_ASSERT(bounds.IsValid());
		return bounds;
	}
}