#pragma once

#include "EditorWidget.h"
#include "ui_StringEditorWidget.h"

namespace CityDraft::UI::Properties
{

	class StringEditorWidget: public EditorWidget
	{
		Q_OBJECT

	public:
		StringEditorWidget(QWidget* parent = nullptr);
		~StringEditorWidget();

	private:
		Ui::StringEditorWidgetClass ui;
	};

}