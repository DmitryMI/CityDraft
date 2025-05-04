#include "IntegerEditorWidget.h"
#include <QIntValidator>
#include <limits>

namespace CityDraft::UI::Properties
{
	IntegerEditorWidget::IntegerEditorWidget(const PropertyGroup& properties, const std::type_info& typeInfo, QWidget* parent):
		LineEditorWidget(properties, parent),
		m_TypeInfo(typeInfo)
	{
		int32_t minVal;
		int32_t maxVal;
		if(m_TypeInfo == typeid(int8_t))
		{
			minVal = std::numeric_limits<int8_t>::min();
			maxVal = std::numeric_limits<int8_t>::max();
		}	
		else if(m_TypeInfo == typeid(uint8_t))
		{
			minVal = std::numeric_limits<uint8_t>::min();
			maxVal = std::numeric_limits<uint8_t>::max();
		}
		else if(m_TypeInfo == typeid(int16_t))
		{
			minVal = std::numeric_limits<int16_t>::min();
			maxVal = std::numeric_limits<int16_t>::max();
		}
		else if(m_TypeInfo == typeid(uint16_t))
		{
			minVal = std::numeric_limits<uint16_t>::min();
			maxVal = std::numeric_limits<uint16_t>::max();
		}
		else if(m_TypeInfo == typeid(int32_t))
		{
			minVal = std::numeric_limits<int32_t>::min();
			maxVal = std::numeric_limits<int32_t>::max();
		}
		else if(m_TypeInfo == typeid(uint32_t))
		{
			// QIntValidator does not support uint32_t
			minVal = 0;
			maxVal = std::numeric_limits<int32_t>::max();
		}
		else if(m_TypeInfo == typeid(int64_t))
		{
			// QIntValidator does not support int64_t
			minVal = std::numeric_limits<int32_t>::min();
			maxVal = std::numeric_limits<int32_t>::max();
		}
		else if(m_TypeInfo == typeid(uint64_t))
		{
			// QIntValidator does not support uint64_t
			minVal = 0;
			maxVal = std::numeric_limits<int32_t>::max();
		}

		QIntValidator* validator = new QIntValidator(minVal, maxVal, this);
		m_Ui.ValueEdit->setValidator(validator);
	}

	std::optional<std::string> IntegerEditorWidget::GetCurrentValueAsString(const PropertyGroup& properties) const
	{
		BOOST_ASSERT(properties[0]->IsReadable());

		if(m_TypeInfo == typeid(int8_t))
		{
			return GetCurrentValueAsStringTemplated<int8_t>(properties);
		}
		if(m_TypeInfo == typeid(uint8_t))
		{
			return GetCurrentValueAsStringTemplated<uint8_t>(properties);
		}
		if(m_TypeInfo == typeid(int16_t))
		{
			return GetCurrentValueAsStringTemplated<int16_t>(properties);
		}
		if(m_TypeInfo == typeid(uint16_t))
		{
			return GetCurrentValueAsStringTemplated<uint16_t>(properties);
		}
		if(m_TypeInfo == typeid(int32_t))
		{
			return GetCurrentValueAsStringTemplated<int32_t>(properties);
		}
		if(m_TypeInfo == typeid(uint32_t))
		{
			return GetCurrentValueAsStringTemplated<uint32_t>(properties);
		}
		if(m_TypeInfo == typeid(int64_t))
		{
			return GetCurrentValueAsStringTemplated<int64_t>(properties);
		}
		if(m_TypeInfo == typeid(uint64_t))
		{
			return GetCurrentValueAsStringTemplated<uint64_t>(properties);
		}

		BOOST_ASSERT(false);
		return std::nullopt;
	}

	bool IntegerEditorWidget::ParseValueFromUi(std::any& outParsedValue) const
	{
		QString text = m_Ui.ValueEdit->text();
		bool ok = false;
		int num = text.toInt(&ok);
		if(!ok)
		{
			return false;
		}

		if(m_TypeInfo == typeid(int8_t))
		{
			outParsedValue = static_cast<int8_t>(num);
			return true;
		}
		if(m_TypeInfo == typeid(uint8_t))
		{
			outParsedValue = static_cast<uint8_t>(num);
			return true;
		}
		if(m_TypeInfo == typeid(int16_t))
		{
			outParsedValue = static_cast<int16_t>(num);
			return true;
		}
		if(m_TypeInfo == typeid(uint16_t))
		{
			outParsedValue = static_cast<uint16_t>(num);
			return true;
		}
		if(m_TypeInfo == typeid(int32_t))
		{
			outParsedValue = static_cast<int32_t>(num);
			return true;
		}
		if(m_TypeInfo == typeid(uint32_t))
		{
			outParsedValue = static_cast<uint32_t>(num);
			return true;
		}
		if(m_TypeInfo == typeid(int64_t))
		{
			outParsedValue = static_cast<int64_t>(num);
			return true;
		}
		if(m_TypeInfo == typeid(uint64_t))
		{
			outParsedValue = static_cast<uint64_t>(num);
			return true;
		}

		return false;
	}
}