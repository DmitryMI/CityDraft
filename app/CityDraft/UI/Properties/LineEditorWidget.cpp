#include "LineEditorWidget.h"


namespace CityDraft::UI::Properties
{
	LineEditorWidget::LineEditorWidget(const PropertyGroup& properties, QWidget* parent)
		: EditorWidget(parent),
		m_Properties(properties)
	{
		m_Ui.setupUi(this);

		BOOST_ASSERT(m_Properties.size() > 0);

		QString propName = QString::fromStdString(std::string(m_Properties[0]->GetName()));
		m_Ui.NameLabel->setText(propName);

		for(const auto& property : m_Properties)
		{
			m_ValueChangedConnections.push_back(property->ConnectToValueChanged(std::bind(&LineEditorWidget::OnValueChanged, this)));
		}

		connect(m_Ui.ValueEdit, &QLineEdit::editingFinished, this, &LineEditorWidget::OnEditingFinished);
	}

	LineEditorWidget::~LineEditorWidget()
	{
		for(const auto& con : m_ValueChangedConnections)
		{
			con.disconnect();
		}
	}

	void LineEditorWidget::Init()
	{
		UpdateLineEdit();
	}

	void LineEditorWidget::OnEditingFinished()
	{
		if(!m_Properties[0]->IsWritable())
		{
			return;
		}

		std::any parsedValue;
		if(!ParseValueFromUi(parsedValue))
		{
			MarkLineEdit(m_Ui.ValueEdit, PropertyFlags::Error);
			return;
		}

		std::vector<boost::signals2::shared_connection_block> blockers;
		for(auto& conn : m_ValueChangedConnections)
		{
			blockers.emplace_back(conn, true);
		}

		for(const auto& property : m_Properties)
		{
			BOOST_ASSERT(property->IsWritable());
			property->SetAny(parsedValue);
		}

		blockers.clear();

		UpdateLineEdit();
	}

	void LineEditorWidget::UpdateLineEdit()
	{
		BOOST_ASSERT(m_Properties[0]->IsWritable() || m_Properties[0]->IsReadable());
		PropertyFlags flags{0};
		if(m_Properties[0]->IsWritable())
		{
			flags = flags | PropertyFlags::Writable;
		}

		if(m_Properties[0]->IsReadable())
		{
			flags = flags | PropertyFlags::Readable;
		}

		if(m_Properties[0]->IsReadable())
		{
			std::optional<std::string> currentValue = GetCurrentValueAsString(m_Properties);
			if(!currentValue)
			{
				flags = flags | PropertyFlags::MultipleValues;
			}
			else
			{
				m_Ui.ValueEdit->setText(QString::fromStdString(currentValue.value()));
			}
		}
		
		MarkLineEdit(m_Ui.ValueEdit, flags);
	}

	void LineEditorWidget::OnValueChanged()
	{
		UpdateLineEdit();
	}

}