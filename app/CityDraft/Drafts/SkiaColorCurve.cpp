#include "CityDraft/Drafts/SkiaColorCurve.h"
#include "CityDraft/Scene.h"

namespace CityDraft::Drafts
{
	void CityDraft::Drafts::SkiaColorCurve::SetTransform(const Transform2D& transform)
	{
		BOOST_ASSERT(m_Curve);
		m_Curve->Transform(transform);
		if(GetScene())
		{
			GetScene()->UpdateDraftModel(this);
		}
	}

}