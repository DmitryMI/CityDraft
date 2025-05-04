#pragma once

#include "LineEditorWidget.h"

namespace CityDraft::UI::Properties
{
	class StringEditorWidget: public LineEditorWidget
	{
		Q_OBJECT

	public:
		StringEditorWidget(const PropertyGroup& properties, QWidget* parent = nullptr): LineEditorWidget(properties, parent)
		{}

		std::optional<std::string> GetCurrentValueAsString(const PropertyGroup& properties) const override;
		bool ParseValueFromUi(std::any& outParsedValue) const override;
	};

}