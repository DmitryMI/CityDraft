#pragma once

#include <boost/signals2.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Layer.h"
#include "CityDraft/Serialization/IArchive.h"
#include "CityDraft/Serialization/ISerializable.h"
#include "CityDraft/Transform2D.h"
#include "CityDraft/Vector2D.h"
#include "IRenderProxy.h"
#include "Properties/Property.h"
#include "Properties/View.h"
#include <type_traits>
#include "CityDraft/Utils/DeduceReturnType.h"

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

		virtual ~Draft();

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

		inline Layer* GetLayer() const
		{
			return m_Layer;
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

		int64_t GetZOrder() const;

		inline std::shared_ptr<IRenderProxy> GetRenderProxy() const
		{
			return m_RenderProxy;
		}

		inline void SetRenderProxy(std::shared_ptr<IRenderProxy> renderProxy)
		{
			m_RenderProxy = renderProxy;
		}

		inline const Properties::Set& GetProperties() const
		{
			return m_Properties;
		}

		// ISerializable
		void Serialize(CityDraft::Serialization::IOutputArchive& archive) const override;
		void Deserialize(CityDraft::Serialization::IInputArchive& archive) override;

	protected:
		boost::signals2::connection m_AssetLoadedConnection;

		virtual void CreateProperties(Properties::Set& properties);

		template<typename Func>
		auto Bind(Func func)
		{
			return std::bind(func, this);
		}

		template<typename TGetFunc, typename TSetFunc, typename TValidateFunc>
		auto MakePropertyView(std::string_view name, TGetFunc getter, TSetFunc setter, TValidateFunc validator)
		{
			using T = CityDraft::Utils::deduce_return_t<TGetFunc>;
			return std::make_shared<Properties::View<T>>(name, this, getter, setter, validator);
		}

		template<typename TGetFunc, typename TSetFunc>
		auto MakePropertyView(std::string_view name, TGetFunc getter, TSetFunc setter)
		{
			using T = CityDraft::Utils::deduce_return_t<TGetFunc>;
			return std::make_shared<Properties::View<T>>(name, this, getter, setter, nullptr);
		}

		template<typename TGetFunc>
		auto MakePropertyView(std::string_view name, TGetFunc getter)
		{
			using T = CityDraft::Utils::deduce_return_t<TGetFunc>;
			return std::make_shared<Properties::View<T>>(name, this, getter, nullptr, nullptr);
		}

	private:
		Layer* m_Layer = nullptr;
		Scene* m_Scene = nullptr;
		CityDraft::Assets::Asset* m_Asset;
		std::shared_ptr<IRenderProxy> m_RenderProxy = nullptr;
		Properties::Set m_Properties;
		
		// Persistent fields
		std::string m_Name{};
		Transform2D m_Transform{};
		int64_t m_ZOrder = 0;

		friend class CityDraft::Scene;
	};
}
