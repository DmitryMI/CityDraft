#pragma once

#include "Selector.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/UI/Rendering/IRenderer.h"
#include <QColor>
#include "CityDraft/AxisAlignedBoundingBox2D.h"

namespace CityDraft::Input::Instruments
{
	Selector::Selector(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QUndoStack* undoStack, QObject* parent):
		Instrument(keyBindingProvider, renderer, undoStack, parent)
	{
		GetLogger()->debug("Created");
	}

	Selector::~Selector()
	{
		GetLogger()->debug("Destroyed");
	}

	EventChainAction Selector::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		BOOST_ASSERT(IsActive());
		BOOST_ASSERT(m_KeyBindingProvider);

		if (event->button() != m_KeyBindingProvider->GetMouseSelectionButton())
		{
			return EventChainAction::Next;
		}

		if (pressed)
		{
			m_FirstMousePosition = event->position();
			m_LastMousePosition = event->position();
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
		BOOST_ASSERT(IsActive());

		QPointF currentPosition = event->position();

		BOOST_ASSERT(m_Renderer);
		m_Renderer->PaintRect(m_FirstMousePosition, currentPosition, QColor(66, 133, 244), 1);
		m_Renderer->Repaint();
		m_LastMousePosition = currentPosition;
		return EventChainAction::Next;
	}

	AxisAlignedBoundingBox2D Selector::GetProjectedSelectionBox() const
	{
		Vector2D first = m_Renderer->Project(m_FirstMousePosition);
		Vector2D second = m_Renderer->Project(m_LastMousePosition);
		Vector2D min{ fmin(first.GetX(), second.GetX()), fmin(first.GetY(), second.GetY()) };
		Vector2D max{ fmax(first.GetX(), second.GetX()), fmax(first.GetY(), second.GetY()) };
		AxisAlignedBoundingBox2D bbox{ min, max };
		return bbox;
	}
}
