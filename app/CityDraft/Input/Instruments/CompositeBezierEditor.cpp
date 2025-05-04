#include "CompositeBezierEditor.h"

namespace CityDraft::Input::Instruments
{
	CompositeBezierEditor::CompositeBezierEditor(const Dependencies& dependencies):
		Instrument(dependencies)
	{
	}

	CompositeBezierEditor::~CompositeBezierEditor()
	{}

	void CompositeBezierEditor::OnPaint()
	{
	}

	EventChainAction CompositeBezierEditor::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		return EventChainAction();
	}

	EventChainAction CompositeBezierEditor::OnRendererMouseMove(QMouseEvent* event)
	{
		return EventChainAction();
	}

	EventChainAction CompositeBezierEditor::OnRendererMouseWheel(QWheelEvent* event)
	{
		return EventChainAction();
	}

	void CompositeBezierEditor::QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions)
	{
	}
}