#pragma once

#include <QWidget>
#include "CityDraft/Drafts/Properties/Property.h"

namespace CityDraft::UI::Properties
{
	class EditorWidget: public QWidget
	{
		Q_OBJECT

	public:
		EditorWidget(const CityDraft::Drafts::Properties::Vector& properties, QWidget* parent = nullptr);
		~EditorWidget() = default;

	protected:
		CityDraft::Drafts::Properties::Vector m_Properties;
	};
}