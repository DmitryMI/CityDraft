#include "ImageDraftEditor.h"


namespace CityDraft::Input::Instruments
{
    ImageDraftEditor::ImageDraftEditor(const InstrumentDependencies& dependencies):
		Instrument(dependencies)
    {
    }

	ImageDraftEditor::~ImageDraftEditor()
	{
	}

	EventChainAction ImageDraftEditor::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		return EventChainAction();
	}

	EventChainAction ImageDraftEditor::OnRendererMouseMove(QMouseEvent* event)
	{
		return EventChainAction();
	}
}
