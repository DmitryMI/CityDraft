#include "StringEditorWidget.h"


namespace CityDraft::UI::Properties
{
	StringEditorWidget::StringEditorWidget(const std::vector<std::shared_ptr<CityDraft::Drafts::Properties::TypedProperty<std::string>>>& properties, QWidget* parent)
		: EditorWidget(parent),
		m_Properties(properties)
	{
		m_Ui.setupUi(this);

		BOOST_ASSERT(m_Properties.size() > 0);

		QString propName = QString::fromStdString(std::string(m_Properties[0]->GetName()));
		m_Ui.NameLabel->setText(propName);

		for(const auto& property : m_Properties)
		{
			m_ValueChangedConnections.push_back(property->ConnectToValueChanged(std::bind(&StringEditorWidget::OnValueChanged, this)));
		}

		UpdateLineEdit();

		connect(m_Ui.StringValueEdit, &QLineEdit::editingFinished, this, &StringEditorWidget::OnEditingFinished);
	}

	StringEditorWidget::~StringEditorWidget()
	{
		for(const auto& con : m_ValueChangedConnections)
		{
			con.disconnect();
		}
	}

	void StringEditorWidget::OnEditingFinished()
	{
		std::vector<boost::signals2::shared_connection_block> blockers;
		for(auto& conn : m_ValueChangedConnections)
		{
			blockers.emplace_back(conn, true);
		}

		QString value = m_Ui.StringValueEdit->text();
		std::string valueStr = value.toStdString();
		for(const auto& property : m_Properties)
		{
			BOOST_ASSERT(property->IsWritable());
			property->Set(valueStr);
		}

		blockers.clear();

		UpdateLineEdit();
	}

	std::optional<std::string> StringEditorWidget::GetCurrentValue() const
	{
		std::string firstValue = m_Properties[0]->Get();
		for(size_t i = 1; i < m_Properties.size(); i++)
		{
			std::string value = m_Properties[i]->Get();
			if(value != firstValue)
			{
				return std::nullopt;
			}
		}

		return firstValue;
	}

	void StringEditorWidget::UpdateLineEdit()
	{
		m_Ui.StringValueEdit->setReadOnly(!m_Properties[0]->IsWritable());
		if(!m_Properties[0]->IsReadable())
		{
			m_Ui.StringValueEdit->setText("");
			m_Ui.StringValueEdit->setPlaceholderText("<write only property>");
			return;
		}

		std::optional<std::string> currentValue = GetCurrentValue();
		if(!currentValue)
		{
			m_Ui.StringValueEdit->setText("");
			m_Ui.StringValueEdit->setPlaceholderText("<multiple values>");
		}
		else
		{
			m_Ui.StringValueEdit->setText(QString::fromStdString(currentValue.value()));
			m_Ui.StringValueEdit->setPlaceholderText("");
		}
	}

	void StringEditorWidget::OnValueChanged()
	{
		UpdateLineEdit();
	}

}