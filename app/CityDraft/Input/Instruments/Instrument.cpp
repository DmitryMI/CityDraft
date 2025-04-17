#include "Instrument.h"
#include "CityDraft/UI/Rendering/IRenderer.h"

namespace CityDraft::Input::Instruments
{
	Instrument::Instrument(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QObject* parent) :
		m_KeyBindingProvider(keyBindingProvider),
		m_Renderer(renderer),
		QObject(parent)
	{
	}

	Instrument::~Instrument()
	{
		
	}

	void Instrument::OnPaint()
	{
	}

	EventChainAction Instrument::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		return EventChainAction::Next;
	}

	EventChainAction Instrument::OnRendererMouseMove(QMouseEvent* event)
	{
		return EventChainAction::Next;
	}

	void Instrument::SetRenderer(CityDraft::UI::Rendering::IRenderer* renderer)
	{
		BOOST_ASSERT(renderer);
		m_Renderer = renderer;
	}

	CityDraft::UI::Rendering::IRenderer* Instrument::GetRenderer() const
	{
		return m_Renderer;
	}

}
