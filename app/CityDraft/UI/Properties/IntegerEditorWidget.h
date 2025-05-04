#pragma once

#include "LineEditorWidget.h"

namespace CityDraft::UI::Properties
{
	class IntegerEditorWidget: public LineEditorWidget
	{
		Q_OBJECT

	public:
		IntegerEditorWidget(const PropertyGroup& properties, const std::type_info& typeInfo, QWidget* parent = nullptr);

		std::optional<std::string> GetCurrentValueAsString(const PropertyGroup& properties) const override;
		bool ParseValueFromUi(std::any& outParsedValue) const override;

		template<typename T>
		std::optional<std::string> GetCurrentValueAsStringTemplated(const PropertyGroup& properties) const
		{
			std::optional<T> value = GetCurrentValue<T>(properties);
			if(!value)
			{
				return std::nullopt;
			}
			return std::to_string(value.value());
		}

	protected:
		const std::type_info& m_TypeInfo;
	};

}