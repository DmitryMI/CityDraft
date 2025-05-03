#pragma once

#include <QWidget>
#include <QUndoStack>
#include <map>
#include "CityDraft/Input/ISelectionManager.h"
#include "EditorWidget.h"
#include "ui_PropertiesWidget.h"

namespace CityDraft::UI::Properties
{

	class PropertiesWidget: public QWidget
	{
		Q_OBJECT

	public:
		PropertiesWidget(CityDraft::Input::ISelectionManager* selectionManager, QUndoStack* undoStack, QWidget* parent = nullptr);
		~PropertiesWidget();

	private:
		Ui::PropertiesWidgetClass ui;

		CityDraft::Input::ISelectionManager* m_SelectionManager;
		QUndoStack* m_UndoStack;
		boost::signals2::connection m_DraftSelectedConnection;
		boost::signals2::connection m_DraftDeselectedConnection;
		std::map<std::string, EditorWidget*> m_EditorWidgets;

		void RemoveEditor(EditorWidget* editor);

		void ClearEditors();
		void UpdateEditors();

		void OnDraftsSelected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts);
		void OnDraftsDeselected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts);
	};

}