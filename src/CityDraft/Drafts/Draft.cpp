#include "CityDraft/Drafts/Draft.h"
#include "CityDraft/Scene.h"

namespace CityDraft::Drafts
{
	Draft::Draft(CityDraft::Assets::Asset* asset):
		m_Asset(asset)
	{
		BOOST_ASSERT(asset);
	}

	bool Draft::IsPointInside(const Vector2D& point)
	{
		return false;
	}

	CityDraft::Assets::Asset* Draft::GetAsset() const
	{
		return m_Asset;
	}

	void Draft::SetTranslation(const Vector2D& translation)
	{
		m_Transform.Translation = translation;
		if (m_Scene)
		{
			m_Scene->UpdateObjectModel(this);
			return;
		}
	}
}
