#pragma once

#include "IWidthProvider.h"

namespace CityDraft::Curves
{
	class ConstWidthProvider : public IWidthProvider
	{
	public:
		ConstWidthProvider(double width) : m_Width(width)
		{

		}

		/// <inheritdoc cref="IWidthProvider.GetWidth"/>
		double GetWidth(double length) const override
		{
			return m_Width;
		}

	private:
		double m_Width;
	};
}