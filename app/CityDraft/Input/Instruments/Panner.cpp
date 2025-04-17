#include "Panner.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/UI/Rendering/IRenderer.h"

namespace CityDraft::Input::Instruments
{
	Panner::Panner(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QObject* parent):
		Instrument(keyBindingProvider, renderer, parent)
	{
	}

	EventChainAction Panner::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		BOOST_ASSERT(m_KeyBindingProvider);

		if (event->button() != m_KeyBindingProvider->GetMouseViewportPanningButton())
		{
			return EventChainAction::Next;
		}

		if (pressed)
		{
			m_LastMousePosition = event->position();
		}
		else
		{
			emit Finished(this);
			
		}
		return EventChainAction::Next;
	}

	EventChainAction Panner::OnRendererMouseMove(QMouseEvent* event)
	{
		QPointF currentPos = event->position();
		QPointF delta = currentPos - m_LastMousePosition;
		m_LastMousePosition = currentPos;

		Vector2D viewportCenter = m_Renderer->GetViewportCenter();
		double viewportZoom = m_Renderer->GetViewportZoom();

		Vector2D deltaScaled = Vector2D(delta.x(), delta.y()) / viewportZoom;
		viewportCenter -= deltaScaled;
		m_Renderer->SetViewportTransform(viewportCenter, viewportZoom);
		m_Renderer->Repaint();

		return EventChainAction::Next;
	}

}
