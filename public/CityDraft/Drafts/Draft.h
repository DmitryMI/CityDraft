#pragma once

#include "CityDraft/Transform2D.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Layer.h"
#include <string>
#include <memory>

namespace CityDraft
{
	class Scene;
}

namespace CityDraft::Assets
{
	class Asset;
}

namespace CityDraft::Drafts
{

	class Draft
	{
	public:
		Draft(CityDraft::Assets::Asset* asset);

		virtual bool IsPointInside(const Vector2D& point);
		virtual AxisAlignedBoundingBox2D GetAxisAlignedBoundingBox() const = 0;

		CityDraft::Assets::Asset* GetAsset() const;

		inline const std::string& GetName() const
		{
			return m_Name;
		}

		inline void SetName(const std::string& name)
		{
			m_Name = name;
		}

		inline const Vector2D& GetTranslation() const
		{
			return m_Transform.Translation;
		}

		virtual void SetTranslation(const Vector2D& translation);

		inline const Transform2D& GetTransform() const
		{
			return m_Transform;
		}

	private:
		Layer* m_Layer = nullptr;
		Scene* m_Scene = nullptr;
		
		int m_ZOrder = 0;
		std::string m_Name{};
		CityDraft::Assets::Asset* m_Asset;

		Transform2D m_Transform{};

		friend class CityDraft::Scene;
	};
}
