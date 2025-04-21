#pragma once

#include "CityDraft/Transform2D.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Layer.h"
#include <string>
#include <memory>
#include <boost/signals2.hpp>
#include "CityDraft/Serialization/IArchive.h"
#include "CityDraft/Serialization/ISerializable.h"
#include <cstdint>

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
	/// <summary>
	/// Represents and Asset, instantiated on the Scene. Can be moved, rotated, scaled and exists in multiple copies.
	/// </summary>
	class Draft : public CityDraft::Serialization::ISerializable
	{
	public:
		/// <summary>
		/// Creates a Draft and sets pointer to parent Asset. To create a Draft, use Asset::CreateDraft() instead.
		/// </summary>
		/// <param name="asset">Parent Asset</param>
		Draft(CityDraft::Assets::Asset* asset);

		virtual bool IsPointInside(const Vector2D& point);

		/// <summary>
		/// Gets Bounding Box, taking Draft's transformation into account.
		/// </summary>
		/// <returns>Bounding Box</returns>
		virtual AxisAlignedBoundingBox2D GetAxisAlignedBoundingBox() const = 0;

		/// <summary>
		/// Gets Parent Asset
		/// </summary>
		/// <returns>Parent Asset</returns>
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

		/// <summary>
		/// Called when Parent Asset is loaded.
		/// </summary>
		/// <param name="asset">Parent Asset</param>
		/// <param name="loadingOk">True if loaded, false if loading failed</param>
		virtual inline void OnAssetLoaded(Assets::Asset* asset, bool loadingOk)
		{

		}

		virtual void SetTranslation(const Vector2D& translation);

		inline const Transform2D& GetTransform() const
		{
			return m_Transform;
		}

		virtual void SetTransform(const Transform2D& transform);

		inline Scene* GetScene() const
		{
			return m_Scene;
		}

		// ISerializable
		void Serialize(CityDraft::Serialization::IOutputArchive& archive) const override;
		void Deserialize(CityDraft::Serialization::IInputArchive& archive) override;

		void SetZOrder(int64_t zOrder);
		int64_t GetZOrder() const;

	protected:
		boost::signals2::connection m_AssetLoadedConnection;

	private:
		Layer* m_Layer = nullptr;
		Scene* m_Scene = nullptr;
		CityDraft::Assets::Asset* m_Asset;
		
		std::string m_Name{};
		Transform2D m_Transform{};
		int64_t m_ZOrder;

		friend class CityDraft::Scene;
	};
}
