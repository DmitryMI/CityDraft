#include "PropertiesWidget.h"

namespace CityDraft::UI::Properties
{

	PropertiesWidget::PropertiesWidget(CityDraft::Input::ISelectionManager* selectionManager, QUndoStack* undoStack, QWidget* parent):
		QWidget(parent),
		m_SelectionManager(selectionManager),
		m_UndoStack(undoStack)
	{
		m_DraftSelectedConnection = selectionManager->ConnectToDraftsSelected(std::bind(&PropertiesWidget::OnDraftsSelected, this, std::placeholders::_1));
		m_DraftDeselectedConnection = selectionManager->ConnectToDraftsDeselected(std::bind(&PropertiesWidget::OnDraftsDeselected, this, std::placeholders::_1));
	
		std::vector<std::shared_ptr<CityDraft::Drafts::Draft>> selectedDrafts;
		const auto& selectedDraftsSet = selectionManager->GetSelectedDrafts();
		std::transform(selectedDraftsSet.begin(), selectedDraftsSet.end(), std::back_inserter(selectedDrafts), [](const auto& item) {return item; });
		OnDraftsSelected(selectedDrafts);
	}

	PropertiesWidget::~PropertiesWidget()
	{
		m_DraftSelectedConnection.disconnect();
		m_DraftDeselectedConnection.disconnect();
	}

	void PropertiesWidget::RemoveEditor(EditorWidget* editor)
	{
	}

	void PropertiesWidget::ClearEditors()
	{
		for (const auto& [name, editor] : m_EditorWidgets)
		{
			RemoveEditor(editor);
			delete editor;
		}

		m_EditorWidgets.clear();
	}

	void PropertiesWidget::UpdateEditors()
	{
		ClearEditors();
		
		const auto& selectedDrafts = m_SelectionManager->GetSelectedDrafts();
		std::vector<CityDraft::Drafts::Properties::Set> draftPropertySets;
		std::transform(selectedDrafts.begin(), selectedDrafts.end(), std::back_inserter(draftPropertySets), [](auto& draft) {return draft->GetProperties(); });
		
		auto commonProps = CityDraft::Drafts::Properties::FindCommonProperties(draftPropertySets);
		// TODO
	}

	void PropertiesWidget::OnDraftsSelected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts)
	{
	}

	void PropertiesWidget::OnDraftsDeselected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts)
	{
	}

}