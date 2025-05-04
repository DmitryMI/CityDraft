#include "CompositeBezierEditor.h"
#include "CityDraft/Input/ISelectionManager.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/UI/Rendering/IRenderer.h"
#include "CityDraft/UI/Colors/IColorsProvider.h"
#include <QWidget>

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
		const auto& selectedDrafts = m_SelectionManager->GetSelectedDrafts();
		for(const auto& draft : selectedDrafts)
		{
			auto curveDraft = dynamic_pointer_cast<CityDraft::Drafts::Curve>(draft);
			if(!curveDraft)
			{
				continue;
			}
			auto bezier = dynamic_pointer_cast<CityDraft::Curves::CompositeBezierCurve>(curveDraft->GetCurve());
			if(bezier)
			{
				PaintAnchors(bezier.get());
			}
		}
	}

	EventChainAction CompositeBezierEditor::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		if(event->button() == m_KeyBindingProvider->GetMouseSelectionButton())
		{
			if(!m_ActiveCurve)
			{
				return EventChainAction::Next;
			}

			if(pressed)
			{
				if(!m_DragActive)
				{
					m_DragActive = true;
					m_FirstPoint = event->position();
					m_PreviousPoint = event->position();
				}
			}
			else
			{
				m_DragActive = false;
			}

			return EventChainAction::Stop;
		}
		else if(event->button() == m_KeyBindingProvider->GetMouseBezierAddAnchorButton())
		{
			if(!pressed)
			{
				InsertAnchor(event);
			}
		}

		return EventChainAction::Next;
	}

	EventChainAction CompositeBezierEditor::OnRendererMouseMove(QMouseEvent* event)
	{
		QObject* parentObj = parent();
		QWidget* parentWidget = dynamic_cast<QWidget*>(parentObj);
		BOOST_ASSERT(parentWidget);

		if(m_DragActive)
		{
			parentWidget->setCursor(Qt::ClosedHandCursor);
			Drag(event);
			m_PreviousPoint = event->position();
			m_Renderer->Repaint();
			return EventChainAction::Stop;
		}
		else
		{
			if(!DetectAnchorComponentUnderCursor(event, m_ActiveDraft, m_ActiveCurve, m_ActiveAnchorIndex, m_ActiveAnchorComponentIndex))
			{
				parentWidget->unsetCursor();
				m_ActiveCurve = nullptr;
				return EventChainAction::Next;
			}
			parentWidget->setCursor(Qt::OpenHandCursor);
		}
		

		return EventChainAction::Stop;
	}

	EventChainAction CompositeBezierEditor::OnRendererMouseWheel(QWheelEvent* event)
	{
		return EventChainAction();
	}

	void CompositeBezierEditor::QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions)
	{
	}

	void CompositeBezierEditor::PaintAnchors(CityDraft::Curves::CompositeBezierCurve* curve)
	{
		BOOST_ASSERT(curve);
		const auto& anchors = curve->GetAnchors();

		double anchorPointSize = CurveAnchorPointSize / m_Renderer->GetViewportZoom();
		double handlePointSize = CurveHandlePointSize / m_Renderer->GetViewportZoom();
		double anchorPointOutlineThickness = 2 / m_Renderer->GetViewportZoom();
		double lineThickness = 0;
		for(const auto& anchor : anchors)
		{
			Vector2D anchorPos = anchor.Position;
			m_Renderer->PaintRect(
				anchorPos - Vector2D{anchorPointSize, anchorPointSize},
				anchorPos + Vector2D{anchorPointSize, anchorPointSize},
				m_ColorsProvider->GetCurveAnchorPointOutlineColor(),
				anchorPointOutlineThickness,
				m_ColorsProvider->GetCurveAnchorPointFillColor()
			);

			Vector2D handleInPos = anchorPos + anchor.IncomingHandle;
			m_Renderer->PaintRect(
				handleInPos - Vector2D{handlePointSize, handlePointSize},
				handleInPos + Vector2D{handlePointSize, handlePointSize},
				m_ColorsProvider->GetCurveHandlePointColor()
			);

			Vector2D handleOutPos = anchorPos + anchor.OutgoingHandle;
			m_Renderer->PaintRect(
				handleOutPos - Vector2D{handlePointSize, handlePointSize},
				handleOutPos + Vector2D{handlePointSize, handlePointSize},
				m_ColorsProvider->GetCurveHandlePointColor()
			);

			m_Renderer->PaintLine(
				handleInPos,
				handleOutPos,
				m_ColorsProvider->GetCurveHandleLineColor(),
				lineThickness
			);
		}
	}

	bool CompositeBezierEditor::DetectAnchorComponentUnderCursor(
		QMouseEvent* event,
		CityDraft::Drafts::Curve*& outDraft,
		CityDraft::Curves::CompositeBezierCurve*& outCurve, 
		size_t& outAnchorIndex,
		size_t& outAnchorComponentIndex
	)
	{
		Vector2D projected = m_Renderer->Project(event->position());

		const auto& selectedDrafts = m_SelectionManager->GetSelectedDrafts();
		for(const auto& draft : selectedDrafts)
		{
			auto curveDraft = dynamic_pointer_cast<CityDraft::Drafts::Curve>(draft);
			if(!curveDraft)
			{
				continue;
			}
			auto bezier = dynamic_pointer_cast<CityDraft::Curves::CompositeBezierCurve>(curveDraft->GetCurve());
			if(!bezier)
			{
				continue;
			}

			if(DetectAnchorComponentInCurveUnderCursor(event, projected, bezier.get(), outAnchorIndex, outAnchorComponentIndex))
			{
				outCurve = bezier.get();
				outDraft = curveDraft.get();
				return true;
			}
		}

		return false;
	}

	bool CompositeBezierEditor::DetectAnchorComponentInCurveUnderCursor(
		QMouseEvent* event,
		Vector2D& projected, 
		CityDraft::Curves::CompositeBezierCurve* curve,
		size_t& outAnchorIndex,
		size_t& outAnchorComponentIndex
	)
	{
		double distanceMax = CurveAnchorPointSize * 2 / m_Renderer->GetViewportZoom();
		const auto& anchors = curve->GetAnchors();
		for(size_t anchorIndex = 0; anchorIndex < anchors.size(); anchorIndex++)
		{
			const auto& anchor = anchors[anchorIndex];
			std::array<Vector2D, 3> components{
				anchor.Position + anchor.IncomingHandle,
				anchor.Position + anchor.OutgoingHandle,
				anchor.Position
				};

			size_t minI = anchorIndex == 0 ? 1 : 0;

			for(size_t i = minI; i < components.size(); i++)
			{
				if(anchorIndex == anchors.size() - 1 && i == 1)
				{
					continue;
				}

				const auto& pos = components[i];
				
				if((projected - pos).GetSize() < distanceMax)
				{
					outAnchorIndex = anchorIndex;
					outAnchorComponentIndex = i;
					return true;
				}
			}
		}
		return false;
	}

	void CompositeBezierEditor::Drag(QMouseEvent* event)
	{
		Vector2D first = m_Renderer->Project(m_PreviousPoint);
		Vector2D second = m_Renderer->Project(event->position());
		Vector2D delta = second - first;

		auto anchor = m_ActiveCurve->GetAnchors()[m_ActiveAnchorIndex];
		switch(m_ActiveAnchorComponentIndex)
		{
		case 0:
			// Incoming handle
			DragHandles(delta, anchor.IncomingHandle, anchor.OutgoingHandle);
			break;
		case 1:
			// Outgoind handle
			DragHandles(delta, anchor.OutgoingHandle, anchor.IncomingHandle);
			break;
		case 2:
			// Anchor pivot
			anchor.Position += delta;
			break;
		default:
			BOOST_ASSERT(false);
			break;
		}

		if(m_ActiveAnchorIndex == 0)
		{
			anchor.IncomingHandle = {0, 0};
		}
		else if(m_ActiveAnchorIndex == m_ActiveCurve->GetAnchors().size() - 1)
		{
			anchor.OutgoingHandle = {0, 0};
		}

		m_ActiveCurve->ChangeAnchor(m_ActiveAnchorIndex, anchor);
		m_Scene->UpdateDraftModel(m_ActiveDraft);
		// m_Renderer->Repaint();
	}

	void CompositeBezierEditor::DragHandles(const Vector2D& delta, Vector2D& handle, Vector2D& oppositeHandle)
	{
		handle += delta;
		oppositeHandle -= delta;
	}

	void CompositeBezierEditor::InsertAnchor(QMouseEvent* event)
	{
		const auto& selectedDrafts = m_SelectionManager->GetSelectedDrafts();
		CityDraft::Drafts::Curve* curveDraft = nullptr;
		for(const auto& draft : selectedDrafts)
		{
			if(auto* curveDraftSelected = dynamic_cast<CityDraft::Drafts::Curve*>(draft.get()))
			{
				if(curveDraft)
				{
					return;
				}
				curveDraft = curveDraftSelected;
			}
		}

		if(!curveDraft)
		{
			return;
		}

		auto* curve = dynamic_cast<CityDraft::Curves::CompositeBezierCurve*>(curveDraft->GetCurve().get());
		if(!curve)
		{
			return;
		}

		Vector2D projected = m_Renderer->Project(event->position());

		double closestT = curve->GetClosestParameter(projected);
		Vector2D closestPoint = curve->GetPoint(closestT);
		const auto width = curveDraft->GetFillWidth();
		if((closestPoint - projected).GetSize() < width->GetWidth(closestT))
		{
			curve->InsertAnchor(closestT);
		}
		else if(closestT > 0.5)
		{
			auto prevAnchor = curve->GetAnchors()[curve->GetAnchors().size() - 1];
			prevAnchor.OutgoingHandle = -prevAnchor.IncomingHandle;
			curve->ChangeAnchor(curve->GetAnchors().size() - 1, prevAnchor);
			
			CityDraft::Curves::CompositeBezierCurve::Anchor anchor;
			anchor.Position = projected;
			anchor.IncomingHandle = (prevAnchor.Position + projected) / 2 - projected;
			curve->AddAnchor(anchor);
		}
		else
		{
			auto prevAnchor = curve->GetAnchors()[0];
			prevAnchor.IncomingHandle = -prevAnchor.OutgoingHandle;
			curve->ChangeAnchor(0, prevAnchor);

			CityDraft::Curves::CompositeBezierCurve::Anchor anchor;
			anchor.Position = projected;
			anchor.OutgoingHandle = (prevAnchor.Position + projected) / 2 - projected;
			curve->AddAnchor(anchor, false);
		}

		m_Scene->UpdateDraftModel(curveDraft);
	}
}