#pragma once

#include "Selector.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/UI/Rendering/IRenderer.h"
#include <QColor>

namespace CityDraft::Input::Instruments
{
	Selector::Selector(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QObject* parent):
		Instrument(keyBindingProvider, renderer, parent)
	{
		GetLogger()->debug("Created");
	}

	Selector::~Selector()
	{
		GetLogger()->debug("Destroyed");
	}

	EventChainAction Selector::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		BOOST_ASSERT(m_KeyBindingProvider);

		if (event->button() != m_KeyBindingProvider->GetMouseSelectionButton())
		{
			return EventChainAction::Next;
		}

		if (pressed)
		{
			m_FirstMousePosition = event->position();
		}
		else
		{
			m_Renderer->Repaint();
			emit Finished(this);
		}

		return EventChainAction::Stop;
	}

	EventChainAction Selector::OnRendererMouseMove(QMouseEvent* event)
	{
		QPointF currentPosition = event->position();

		BOOST_ASSERT(m_Renderer);
		m_Renderer->PaintRect(m_FirstMousePosition, currentPosition, QColor(66, 133, 244), 1);
		m_Renderer->Repaint();
		return EventChainAction::Next;
	}
}
