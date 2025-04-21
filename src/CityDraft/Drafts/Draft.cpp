#include "CityDraft/Drafts/Draft.h"
#include "CityDraft/Scene.h"

namespace CityDraft::Drafts
{
	Draft::Draft(CityDraft::Assets::Asset* asset):
		m_Asset(asset)
	{
		BOOST_ASSERT(asset);

		m_AssetLoadedConnection = asset->ConnectToAssetLoadedEvent(std::bind(&Draft::OnAssetLoaded, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool Draft::IsPointInside(const Vector2D& point)
	{
		return GetAxisAlignedBoundingBox().Contains(point);
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
			m_Scene->UpdateDraftModel(this);
			return;
		}
	}

	void Draft::SetTransform(const Transform2D& transform)
	{
		m_Transform = transform;
		if (m_Scene)
		{
			m_Scene->UpdateDraftModel(this);
			return;
		}
	}

	void Draft::Serialize(CityDraft::Serialization::IOutputArchive& archive) const
	{
		archive << m_Name;
		archive << m_Transform;
		archive << m_ZOrder;
	}

	void Draft::Deserialize(CityDraft::Serialization::IInputArchive& archive)
	{
		archive >> m_Name;
		archive >> m_Transform;
		archive >> m_ZOrder;
	}

	int64_t Draft::GetZOrder() const
	{
		return m_ZOrder;
	}
}
