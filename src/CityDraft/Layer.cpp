#include "CityDraft/Layer.h"
#include "CityDraft/Scene.h"

namespace CityDraft
{
	void Layer::SetVisible(bool visible)
	{
		BOOST_ASSERT(m_Scene);
		m_Scene->SetLayerVisibile(this, visible);
	}

	void Layer::SetLocked(bool locked)
	{
		BOOST_ASSERT(m_Scene);
		m_Scene->SetLayerLocked(this, locked);
	}

	void Layer::SetName(std::string_view name)
	{
		BOOST_ASSERT(m_Scene);
		m_Scene->RenameLayer(this, std::string(name));
	}
}