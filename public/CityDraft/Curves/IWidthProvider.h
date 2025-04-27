#pragma once

#include "CityDraft/Serialization/ISerializable.h"

namespace CityDraft::Curves
{
	class IWidthProvider : public CityDraft::Serialization::ISerializable
	{
	public:
		virtual ~IWidthProvider() = default;

		/// <summary>
		/// Gets Curve width at given length along the Curve
		/// </summary>
		/// <param name="length">Length along the Curve</param>
		/// <returns>Width</returns>
		virtual double GetWidth(double length) const = 0;

		/// <summary>
		/// Type reflection. Do not override it manually, use REGISTER_WIDTH_TYPE() macro instead.
		/// </summary>
		/// <returns>Human-readable type name in format 'class CityDraft::Curves::CLASS_NAME'</returns>
		virtual std::string GetTypeName() const = 0;
	};
}