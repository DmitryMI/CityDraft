#include "CityDraft/Drafts/Curve.h"
#include "CityDraft/Curves/Factory.h"

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
		BOOST_ASSERT(m_Curve);
		return m_Curve->GetBoundingBox();
	}

	void Curve::OnAssetLoaded(Assets::Asset* asset, bool loadingOk)
	{
	}

	void Curve::Serialize(CityDraft::Serialization::IOutputArchive& archive) const
	{
		Draft::Serialize(archive);
		archive << m_Curve->GetCurveTypeName();
		archive << *m_Curve;
	}

	void Curve::Deserialize(CityDraft::Serialization::IInputArchive& archive)
	{
		Draft::Deserialize(archive);

		std::string curveTypeName;
		archive >> curveTypeName;
		m_Curve = CityDraft::Curves::Factory::GetInstance().Create(curveTypeName);
		BOOST_ASSERT(m_Curve);
		archive >> *m_Curve;
	}

}