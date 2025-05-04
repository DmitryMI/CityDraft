#pragma once

#include <optional>
#include "EditorWidget.h"
#include "ui_LineEditorWidget.h"
#include "CityDraft/Drafts/Properties/TypedProperty.h"
#include <boost/signals2.hpp>
#include <any>
#include <QLineEdit>

namespace CityDraft::UI::Properties
{

	using PropertyGroup = CityDraft::Drafts::Properties::Vector;

	class LineEditorWidget: public EditorWidget
	{
		Q_OBJECT

	public:
		LineEditorWidget(const PropertyGroup& properties, QWidget* parent = nullptr);
		~LineEditorWidget();

		inline std::string_view GetPropertyName() const override
		{
			BOOST_ASSERT(m_Properties.size() > 0);
			return m_Properties[0]->GetName();
		}

		void Init();

	protected:
		Ui::LineEditorWidgetClass m_Ui;
		PropertyGroup m_Properties;
		virtual std::optional<std::string> GetCurrentValueAsString(const PropertyGroup& properties) const = 0;
		virtual bool ParseValueFromUi(std::any& outParsedValue) const = 0;

	private slots:
		void OnEditingFinished();

	private:

		std::vector<boost::signals2::connection> m_ValueChangedConnections;

		void UpdateLineEdit();
		void OnValueChanged();
	};

}