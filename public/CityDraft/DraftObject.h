#pragma once

#include "Transform2D.h"
#include "Vector2D.h"
#include "DraftLayer.h"
#include <memory>

namespace CityDraft
{
	class Asset;
	class DraftScene;

	class DraftObject
	{
	public:
		virtual bool IsPointInside(const Vector2D& point);
		virtual Asset* GetAsset() const = 0;

		inline const std::string& GetName() const
		{
			return m_Name;
		}

	private:
		std::weak_ptr<DraftLayer> m_Layer;
		DraftScene* m_Scene;
		
		Transform2D m_Transform;
		int m_ZOrder = 0;
		std::string m_Name;

		friend class DraftScene;
	};
}
