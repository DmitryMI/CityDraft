#pragma once

#include "IWidthProvider.h"
#include "CityDraft/Serialization/IArchive.h"
#include "CityDraft/Curves/Factory.h"

namespace CityDraft::Curves
{
	class ConstWidthProvider : public IWidthProvider
	{
	public:
		ConstWidthProvider(): m_Width(0)
		{

		}

		ConstWidthProvider(double width) : m_Width(width)
		{

		}

		double GetWidth(double length) const override
		{
			return m_Width;
		}

		inline void Serialize(CityDraft::Serialization::IOutputArchive& archive) const override
		{
			archive << m_Width;
		}

		inline void Deserialize(CityDraft::Serialization::IInputArchive& archive) override
		{
			archive >> m_Width;
		}

	private:
		double m_Width;

		REGISTER_TYPE(ConstWidthProvider)
	};
}