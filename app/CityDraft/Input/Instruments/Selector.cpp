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

		m_DraftRemovedConnection = dependencies.Scene->ConnectToDraftRemoved(std::bind(&Selector::OnDraftRemoved, this, std::placeholders::_1));
	}

	Selector::~Selector()
	{
		m_DraftRemovedConnection.disconnect();
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
			m_IsMultiSelection = false;
			m_SelectionPressed = true;
		}
		else
		{
			FinishSelection(event);
			m_SelectionPressed = false;
			m_IsMultiSelection = false;
			// emit Finished(this);
		}

		m_Renderer->Repaint();

		return EventChainAction::Stop;
	}

	EventChainAction Selector::OnRendererMouseMove(QMouseEvent* event)
	{
		BOOST_ASSERT(IsActive());
		BOOST_ASSERT(m_Renderer);

		QPointF currentPosition = event->position();

		if (m_SelectionPressed)
		{
			QPointF delta = currentPosition - m_FirstMousePosition;
			if (fabs(delta.x()) > MultiSelectMouseMoveThreshold || fabs(delta.y()) > MultiSelectMouseMoveThreshold)
			{
				m_IsMultiSelection = true;
			}

			if (m_IsMultiSelection)
			{
				m_Renderer->PaintRectViewportSpace(m_FirstMousePosition, currentPosition, m_ColorsProvider->GetSelectionBoxColor(), 1.0);
				m_Renderer->Repaint();
			}
		}
		else
		{
			Vector2D cursorProjected = m_Renderer->Project(m_LastMousePosition);
			auto* draft = m_Scene->QueryHighestDraft(cursorProjected, CityDraft::Scene::QueryParams()).get();
			if (draft != m_DraftUnderCursor)
			{
				m_DraftUnderCursor = draft;
				m_Renderer->Repaint();
			}
		}

		m_LastMousePosition = currentPosition;
		return EventChainAction::Next;
	}

	void Selector::OnPaint()
	{
		if (m_IsMultiSelection)
		{
			std::vector<std::shared_ptr<Drafts::Draft>> drafts;
			AxisAlignedBoundingBox2D bbox = GetProjectedSelectionBox();
			m_Scene->QueryDrafts(bbox, CityDraft::Scene::QueryParams(), drafts);
			for (const auto& draft : drafts)
			{
				auto draftBbox = draft->GetAxisAlignedBoundingBox();
				m_Renderer->PaintRect(draftBbox.GetMin(), draftBbox.GetMax(), m_ColorsProvider->GetDraftPreSelectionBoxColor(), 1.0 / m_Renderer->GetViewportZoom());
			}
		}
		else
		{
			const auto& selected = m_SelectionManager->GetSelectedDrafts();
			if (m_DraftUnderCursor && !(selected.size() == 1 && selected.begin()->get() == m_DraftUnderCursor))
			{
				auto draftBbox = m_DraftUnderCursor->GetAxisAlignedBoundingBox();
				m_Renderer->PaintRect(draftBbox.GetMin(), draftBbox.GetMax(), m_ColorsProvider->GetDraftPreSelectionBoxColor(), 1.0 / m_Renderer->GetViewportZoom());
				m_Renderer->Repaint();
			}
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
		m_Scene->QueryDrafts(bbox, CityDraft::Scene::QueryParams(), drafts);

		return drafts.size() - draftsNum;
	}

	void Selector::QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions)
	{
		ToolDescryptor selectTool;
		selectTool.MouseButton = m_KeyBindingProvider->GetMouseSelectionButton();
		ToolDescryptor selectAdditiveTool;
		selectAdditiveTool.MouseButton = m_KeyBindingProvider->GetMouseSelectionButton();
		selectAdditiveTool.Modifier = m_KeyBindingProvider->GetSelectionAdditiveModifier();

		if (!toolDescriptions.contains(selectTool))
		{
			toolDescriptions[selectTool] = "select";
		}

		if (!toolDescriptions.contains(selectAdditiveTool))
		{
			toolDescriptions[selectAdditiveTool] = "add to selection";
		}
	}

	bool Selector::HasActiveTool() const
	{
		return m_SelectionPressed;
	}

	void Selector::FinishSelection(QMouseEvent* event)
	{
		m_Renderer->Repaint();

		std::vector<std::shared_ptr<Drafts::Draft>> drafts;
		if (m_IsMultiSelection)
		{
			const auto& bbox = GetProjectedSelectionBox();
			m_Scene->QueryDrafts(bbox, CityDraft::Scene::QueryParams(), drafts);
		}
		else
		{
			Vector2D cursorProjected = m_Renderer->Project(m_LastMousePosition);
			std::shared_ptr<Drafts::Draft> draft = m_Scene->QueryHighestDraft(cursorProjected, CityDraft::Scene::QueryParams());
			if (draft)
			{
				drafts.push_back(draft);
			}
		}

		if (!event->modifiers().testFlag(m_KeyBindingProvider->GetSelectionAdditiveModifier()))
		{
			m_SelectionManager->ClearSelectedDrafts();
		}
		m_SelectionManager->AddDraftsToSelection(drafts);
	}

	void Selector::OnDraftRemoved(CityDraft::Drafts::Draft* draft)
	{
		if(m_DraftUnderCursor == draft)
		{
			m_DraftUnderCursor = nullptr;
		}
	}
}
