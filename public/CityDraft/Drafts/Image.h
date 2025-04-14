#pragma once

#include "Draft.h"

namespace CityDraft::Drafts
{
	class Image : public Draft
	{
	public:
		AxisAlignedBoundingBox2D GetAxisAlignedBoundingBox() const override;
	};
}
