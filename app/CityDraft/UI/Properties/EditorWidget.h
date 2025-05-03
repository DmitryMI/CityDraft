#pragma once

#include <QWidget>
#include "CityDraft/Drafts/Properties/Property.h"

namespace CityDraft::UI::Properties
{
	class EditorWidget: public QWidget
	{
		Q_OBJECT

	public:
		EditorWidget(QWidget* parent = nullptr);
		~EditorWidget() = default;
		virtual std::string_view GetPropertyName() const = 0;

	protected:
	};
}