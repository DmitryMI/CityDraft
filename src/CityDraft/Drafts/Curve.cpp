#include "CityDraft/Drafts/Curve.h"

namespace CityDraft::Drafts
{
	std::shared_ptr<CityDraft::Curves::ICurve> Curve::GetCurve() const
	{
		return m_Curve;
	}

	void Curve::SetCurve(std::shared_ptr<CityDraft::Curves::ICurve> curve)
	{
		m_Curve = curve;
	}

	std::shared_ptr<CityDraft::Curves::IWidthProvider> Curve::GetFillWidth() const
	{
		return m_FillWidth;
	}

	void Curve::SetFillWidth(std::shared_ptr<CityDraft::Curves::IWidthProvider> width)
	{
		m_FillWidth = width;
	}

	std::shared_ptr<CityDraft::Curves::IWidthProvider> Curve::GetOutlineWidth() const
	{
		return m_OutlineWidth;
	}

	void Curve::SetOutlineWidth(std::shared_ptr<CityDraft::Curves::IWidthProvider> width)
	{
		m_OutlineWidth = width;
	}

	const CityDraft::LinearColorF& Curve::GetFillColor() const
	{
		return m_FillColor;
	}

	void Curve::SetFillColor(const CityDraft::LinearColorF& color)
	{
		m_FillColor = color;
	}

	const CityDraft::LinearColorF& Curve::GetOutlineColor() const
	{
		return m_OutlineColor;
	}

	void Curve::SetOutlineColor(const CityDraft::LinearColorF& color)
	{
		m_OutlineColor = color;
	}

	AxisAlignedBoundingBox2D Curve::GetAxisAlignedBoundingBox() const
	{
		AxisAlignedBoundingBox2D boxDefault{{-50, -50}, {50, 50}};
		return boxDefault.Transform(GetTransform());
	}

	void Curve::OnAssetLoaded(Assets::Asset* asset, bool loadingOk)
	{
	}


}