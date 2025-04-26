#pragma once

namespace CityDraft::Curves
{
	class IWidthProvider
	{
	public:
		virtual ~IWidthProvider() = default;

		/// <summary>
		/// Gets Curve width at given length along the Curve
		/// </summary>
		/// <param name="length">Length along the Curve</param>
		/// <returns>Width</returns>
		virtual double GetWidth(double length) const = 0;
	};
}