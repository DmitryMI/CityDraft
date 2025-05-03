#pragma once

#include <optional>
#include "EditorWidget.h"
#include "ui_StringEditorWidget.h"
#include "CityDraft/Drafts/Properties/TypedProperty.h"
#include <boost/signals2.hpp>

namespace CityDraft::UI::Properties
{

	using StringPropertyGroup = CityDraft::Drafts::Properties::TypedPropertyGroup<std::string>;

	class StringEditorWidget: public EditorWidget
	{
		Q_OBJECT

	public:
		StringEditorWidget(const StringPropertyGroup& properties, QWidget* parent = nullptr);
		~StringEditorWidget();

		inline std::string_view GetPropertyName() const override
		{
			BOOST_ASSERT(m_Properties.size() > 0);
			return m_Properties[0]->GetName();
		}

	private slots:
		void OnEditingFinished();

	private:
		Ui::StringEditorWidgetClass m_Ui;
		StringPropertyGroup m_Properties;
		std::vector<boost::signals2::connection> m_ValueChangedConnections;

		std::optional<std::string> GetCurrentValue() const;
		void UpdateLineEdit();
		void OnValueChanged();
	};

}