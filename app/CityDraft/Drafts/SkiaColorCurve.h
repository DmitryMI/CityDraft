#pragma once

#include "CityDraft/Drafts/Curve.h"

namespace CityDraft::Drafts
{
	class SkiaColorCurve: public Curve
	{
	public:
		using Curve::Curve;

		void SetTransform(const Transform2D& transform) override;
	};
}
