#include "PropertiesWidget.h"
#include "StringEditorWidget.h"
#include "IntegerEditorWidget.h"
#include "TransformEditorWidget.h"
#include <QVBoxLayout>

namespace CityDraft::UI::Properties
{

	PropertiesWidget::PropertiesWidget(CityDraft::Input::ISelectionManager* selectionManager, QUndoStack* undoStack, QWidget* parent):
		QWidget(parent),
		m_SelectionManager(selectionManager),
		m_UndoStack(undoStack)
	{
		m_Ui.setupUi(this);

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
		ClearEditors();
	}

	void PropertiesWidget::AddEditor(EditorWidget* editor)
	{
		BOOST_ASSERT(!m_Editors.contains(editor->GetPropertyName()));

		QWidget* container = m_Ui.ScrollArea->widget();
		BOOST_ASSERT(container);
		QVBoxLayout* containerLayout = dynamic_cast<QVBoxLayout*>(container->layout());
		BOOST_ASSERT(containerLayout);
		containerLayout->addWidget(editor);
		m_Editors[editor->GetPropertyName()] = editor;
	}

	void PropertiesWidget::RemoveEditor(EditorWidget* editor)
	{
		QWidget* container = m_Ui.ScrollArea->widget();
		BOOST_ASSERT(container);
		QVBoxLayout* containerLayout = dynamic_cast<QVBoxLayout*>(container->layout());
		BOOST_ASSERT(containerLayout);
		containerLayout->removeWidget(editor);
		m_Editors.erase(editor->GetPropertyName());
		delete editor;
	}

	void PropertiesWidget::ClearEditors()
	{
		const auto editors = m_Editors;
		for(const auto& [name, editor] : editors)
		{
			RemoveEditor(editor);
		}
	}

	void PropertiesWidget::UpdateEditors()
	{
		ClearEditors();
		
		const auto& selectedDrafts = m_SelectionManager->GetSelectedDrafts();
		std::vector<CityDraft::Drafts::Properties::Set> draftPropertySets;
		std::transform(selectedDrafts.begin(), selectedDrafts.end(), std::back_inserter(draftPropertySets), [](auto& draft) {return draft->GetProperties(); });
		
		auto commonProps = CityDraft::Drafts::Properties::FindCommonProperties(draftPropertySets);
		if(commonProps.size() == 0)
		{
			return;
		}

		for(const auto& group : commonProps)
		{
			EditorWidget* editor = CreateEditorWidget(group.first, group.second);
			if(!editor)
			{
				continue;
			}
			AddEditor(editor);
		}
	}

	void PropertiesWidget::OnDraftsSelected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts)
	{
		ClearEditors();
		UpdateEditors();
	}

	void PropertiesWidget::OnDraftsDeselected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts)
	{
		ClearEditors();
		UpdateEditors();
	}

	EditorWidget* PropertiesWidget::CreateEditorWidget(std::string_view propertyName, const CityDraft::Drafts::Properties::Vector& properties)
	{
		BOOST_ASSERT(properties.size() > 0);

		if (dynamic_pointer_cast<CityDraft::Drafts::Properties::TypedProperty<std::string>>(properties[0]))
		{
			StringEditorWidget* editor = new StringEditorWidget(properties, this);
			editor->Init();
			return editor;
		}
		if(dynamic_pointer_cast<CityDraft::Drafts::Properties::TypedProperty<int64_t>>(properties[0]))
		{
			IntegerEditorWidget* editor = new IntegerEditorWidget(properties, typeid(int64_t), this);
			editor->Init();
			return editor;
		}
		if(dynamic_pointer_cast<CityDraft::Drafts::Properties::TypedProperty<CityDraft::Transform2D>>(properties[0]))
		{
			auto group = ConvertToPropertyGroup<CityDraft::Transform2D>(properties);
			return new TransformEditorWidget(group, this);
		}
		return nullptr;
	}

	

}