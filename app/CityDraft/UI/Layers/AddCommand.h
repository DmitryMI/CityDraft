#pragma once

#include <QUndoCommand>

class AddCommand: public QUndoCommand
{
public:
	TransformChangeCommand(
		const std::vector<TransformChange>& changes,
		CityDraft::UI::Rendering::IRenderer* renderer,
		QUndoCommand* parent = nullptr
	):
		m_Changes(changes),
		m_Renderer(renderer),
		QUndoCommand("Transform Change", parent)
	{}

	void undo() override
	{
		for(const auto& change : m_Changes)
		{
			change.Draft->SetTransform(change.OldTransform);
		}
		m_Renderer->Repaint();
	}

	void redo() override
	{
		for(const auto& change : m_Changes)
		{
			change.Draft->SetTransform(change.NewTransform);
		}
		m_Renderer->Repaint();
	}

private:
	std::vector<TransformChange> m_Changes;
	CityDraft::UI::Rendering::IRenderer* m_Renderer = nullptr;
};