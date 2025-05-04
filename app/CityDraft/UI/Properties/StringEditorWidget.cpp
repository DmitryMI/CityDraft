#include "StringEditorWidget.h"

namespace CityDraft::UI::Properties
{

	std::optional<std::string> StringEditorWidget::GetCurrentValueAsString(const PropertyGroup& properties) const
	{
		return GetCurrentValue<std::string>(properties);
	}

	bool StringEditorWidget::ParseValueFromUi(std::any& outParsedValue) const
	{
		outParsedValue = m_Ui.ValueEdit->text().toStdString();
		return true;
	}

}