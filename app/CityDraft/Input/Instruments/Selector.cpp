#pragma once

#include "Selector.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/UI/Rendering/IRenderer.h"
#include <QColor>
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/UI/Colors/IColorsProvider.h"
#include "CityDraft/Input/ISelectionManager.h"

namespace CityDraft::Input::Instruments
{
	Selector::Selector(const Dependencies& dependencies):
		Instrument(dependencies)
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
			const auto& bbox = GetProjectedSelectionBox();
			std::vector<std::shared_ptr<Drafts::Draft>> drafts;
			m_Scene->QueryDraftsOnAllLayers(bbox, drafts);
			if (!event->modifiers().testFlag(m_KeyBindingProvider->GetSelectionAdditiveModifier()))
			{
				m_SelectionManager->ClearSelectedDrafts();
			}
			m_SelectionManager->AddDraftsToSelection(drafts);
			emit Finished(this);
		}

		return EventChainAction::Stop;
	}

	EventChainAction Selector::OnRendererMouseMove(QMouseEvent* event)
	{
		BOOST_ASSERT(IsActive());

		QPointF currentPosition = event->position();

		BOOST_ASSERT(m_Renderer);
		m_Renderer->PaintRectViewportSpace(m_FirstMousePosition, currentPosition, QColor(66, 133, 244), 1.0);
		m_Renderer->Repaint();
		m_LastMousePosition = currentPosition;
		return EventChainAction::Next;
	}

	void Selector::OnPaint()
	{
		std::vector<std::shared_ptr<Drafts::Draft>> drafts;
		AxisAlignedBoundingBox2D bbox = GetProjectedSelectionBox();
		m_Scene->QueryDraftsOnAllLayers(bbox, drafts);
		for (const auto& draft : drafts)
		{
			auto draftBbox = draft->GetAxisAlignedBoundingBox();
			m_Renderer->PaintRect(draftBbox.GetMin(), draftBbox.GetMax(), m_ColorsProvider->GetDraftPreSelectionBoxColor(), 1.0 / m_Renderer->GetViewportZoom());
		}
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

	size_t Selector::GetSelectedDrafts(std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts) const
	{
		size_t draftsNum = drafts.size();
		AxisAlignedBoundingBox2D bbox = GetProjectedSelectionBox();
		m_Scene->QueryDraftsOnAllLayers(bbox, drafts);

		return drafts.size() - draftsNum;
	}
}
