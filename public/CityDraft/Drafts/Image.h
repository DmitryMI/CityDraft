#pragma once

#include "Draft.h"

namespace CityDraft::Drafts
{
	class Image : public Draft
	{
	public:
		using Draft::Draft;

		AxisAlignedBoundingBox2D GetAxisAlignedBoundingBox() const override;
	};
}
