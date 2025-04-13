#include "CityDraft/DraftScene.h"

namespace CityDraft
{
	std::shared_ptr<DraftObject> DraftScene::AddDraftObject(DraftObject* obj)
	{
		obj->m_Scene = this;
		if (obj->GetName() == "")
		{
			obj->m_Name = "DraftObject-" + std::to_string(m_Objects.size());
		}
		std::shared_ptr<DraftObject> objPtr(obj);
		m_Objects.push_back(objPtr);
		m_DraftObjectAdded(obj);
		// m_Logger->debug("{} added to scene", obj->GetName());
		return objPtr;
	}

	void DraftScene::RemoveDraftObject(DraftObject* obj)
	{
		if (!obj)
		{
			// m_Logger->error("RemoveDraftObject: obj argument is nullptr");
			return;
		}

		const auto iter = std::find_if(m_Objects.begin(), m_Objects.end(), [obj](const auto& objPtr) {return objPtr.get() == obj; });
		if (iter == m_Objects.end())
		{
			// m_Logger->warn("RemoveDraftObject: {} was not found in m_Objects list.", obj->GetName());
			return;
		}
		m_Objects.erase(iter);
		// m_Logger->debug("{} removed from scene", obj->GetName());
	}
}
