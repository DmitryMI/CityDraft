#pragma once

#include "Draft.h"
#include "CityDraft/Curves/ICurve.h"
#include "CityDraft/Curves/IWidthProvider.h"
#include "CityDraft/LinearColor.h"

namespace Assets
{
	class Asset;
}

namespace CityDraft::Drafts
{
	class Curve: public Draft
	{
	public:
		using Draft::Draft;

		std::shared_ptr<CityDraft::Curves::ICurve> GetCurve() const;
		virtual void SetCurve(std::shared_ptr<CityDraft::Curves::ICurve> curve);
		std::shared_ptr<CityDraft::Curves::IWidthProvider> GetFillWidth() const;
		virtual void SetFillWidth(std::shared_ptr<CityDraft::Curves::IWidthProvider> width);
		std::shared_ptr<CityDraft::Curves::IWidthProvider> GetOutlineWidth() const;
		virtual void SetOutlineWidth(std::shared_ptr<CityDraft::Curves::IWidthProvider> width);
		const CityDraft::LinearColorF& GetFillColor() const;
		virtual void SetFillColor(const CityDraft::LinearColorF& color);
		const CityDraft::LinearColorF& GetOutlineColor() const;
		virtual void SetOutlineColor(const CityDraft::LinearColorF& color);

		// Draft
		AxisAlignedBoundingBox2D GetAxisAlignedBoundingBox() const override;
		void OnAssetLoaded(Assets::Asset* asset, bool loadingOk) override;

		// ISerializable
		void Serialize(CityDraft::Serialization::IOutputArchive& archive) const override;
		void Deserialize(CityDraft::Serialization::IInputArchive& archive) override;

	protected:
		std::shared_ptr<CityDraft::Curves::ICurve> m_Curve;
		std::shared_ptr<CityDraft::Curves::IWidthProvider> m_FillWidth;
		std::shared_ptr<CityDraft::Curves::IWidthProvider> m_OutlineWidth;
		CityDraft::LinearColorF m_FillColor;
		CityDraft::LinearColorF m_OutlineColor;
	};
}
