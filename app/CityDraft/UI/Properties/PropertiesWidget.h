#pragma once

#include <QWidget>
#include <QUndoStack>
#include <map>
#include "CityDraft/Input/ISelectionManager.h"
#include "CityDraft/Drafts/Properties/Property.h"
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
		Ui::PropertiesWidgetClass m_Ui;

		CityDraft::Input::ISelectionManager* m_SelectionManager;
		QUndoStack* m_UndoStack;
		boost::signals2::connection m_DraftSelectedConnection;
		boost::signals2::connection m_DraftDeselectedConnection;
		std::map<std::string_view, EditorWidget*> m_Editors;

		void AddEditor(EditorWidget* editor);
		void RemoveEditor(EditorWidget* editor);

		void ClearEditors();
		void UpdateEditors();

		void OnDraftsSelected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts);
		void OnDraftsDeselected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts);

		EditorWidget* CreateEditorWidget(std::string_view propertyName, const CityDraft::Drafts::Properties::Vector& properties);
		

		template<typename T>
		CityDraft::Drafts::Properties::TypedPropertyGroup<T> ConvertToPropertyGroup(const CityDraft::Drafts::Properties::Vector& properties)
		{
			CityDraft::Drafts::Properties::TypedPropertyGroup<T> group;
			for(const auto& property : properties)
			{
				const auto propertyCast = dynamic_pointer_cast<CityDraft::Drafts::Properties::TypedProperty<T>>(property);
				if(!propertyCast)
				{
					return {};
				}
				group.push_back(propertyCast);
			}

			return group;
		}
	};

}