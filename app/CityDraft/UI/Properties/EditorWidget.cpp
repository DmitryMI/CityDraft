#include "EditorWidget.h"

namespace CityDraft::UI::Properties
{
	EditorWidget::EditorWidget(const CityDraft::Drafts::Properties::Vector& properties, QWidget* parent):
		QWidget(parent), 
		m_Properties(properties)
	{}

}