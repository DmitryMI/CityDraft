#include "ImageDraftEditor.h"
#include "CityDraft/Input/ISelectionManager.h"
#include "CityDraft/UI/Rendering/IRenderer.h"
#include "CityDraft/UI/Colors/IColorsProvider.h"
#include <QWidget>

namespace CityDraft::Input::Instruments
{
    ImageDraftEditor::ImageDraftEditor(const Dependencies& dependencies):
		Instrument(dependencies)
    {
		GetLogger()->debug("Created");
    }

	ImageDraftEditor::~ImageDraftEditor()
	{
		GetLogger()->debug("Destroyed");
	}

	EventChainAction ImageDraftEditor::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		BOOST_ASSERT(IsActive());

		if (pressed)
		{
			if (m_Tool != Tool::None)
			{
				m_ToolInUse = true;
				m_PreviousPoint = event->position();
				m_FirstPoint = m_PreviousPoint;
				return EventChainAction::Stop;
			}
		}
		else
		{
			m_ToolInUse = false;
		}
		
		return EventChainAction();
	}

	EventChainAction ImageDraftEditor::OnRendererMouseMove(QMouseEvent* event)
	{
		BOOST_ASSERT(IsActive());

		if (m_SelectionManager->GetSelectedDrafts().size() == 0)
		{
			return EventChainAction::Next;
		}

		if (!m_ToolInUse)
		{
			DetectTransformationTool(event);
			m_Renderer->Repaint();
			m_PreviousPoint = event->position();
			m_FirstPoint = m_PreviousPoint;
			return EventChainAction::Next;
		}

		switch (m_Tool)
		{
		case Tool::None:
			BOOST_ASSERT(false);
			break;
		case Tool::Drag:
			Drag(event);
			break;
		case Tool::Rotate:
			Rotate(event);
			break;
		case Tool::Scale:
			Scale(event);
			break;
		}

		m_PreviousPoint = event->position();
		m_Renderer->Repaint();
		return EventChainAction::Stop;
	}

	void ImageDraftEditor::OnPaint()
	{
		BOOST_ASSERT(IsActive());
		if (m_SelectionManager->GetSelectedDrafts().size() == 0)
		{
			return;
		}

		PaintRotatorCircle();
	}

	void ImageDraftEditor::OnActiveFlagChanged()
	{
		QObject* parentObj = parent();
		QWidget* parentWidget = dynamic_cast<QWidget*>(parentObj);
		BOOST_ASSERT(parentWidget);
		parentWidget->unsetCursor();
	}

	void ImageDraftEditor::PaintRotatorCircle()
	{
		AxisAlignedBoundingBox2D bbox = GetSelectionBoundingBox();
		Vector2D center;
		double radius;
		bbox.GetCircumcircle(center, radius);
		m_Renderer->PaintRect(bbox.GetMin(), bbox.GetMax(), m_ColorsProvider->GetDraftScaleBoxColor(), 2.0 / m_Renderer->GetViewportZoom());

		QColor circleColor;
		if (m_Tool == Tool::Rotate)
		{
			circleColor = m_ColorsProvider->GetDraftRotationCircleHighlightedColor();
		}
		else
		{
			circleColor = m_ColorsProvider->GetDraftRotationCircleColor();
		}
		m_Renderer->PaintCircle(center, radius, circleColor, 2.0 / m_Renderer->GetViewportZoom());
	}

	void ImageDraftEditor::DetectTransformationTool(QMouseEvent* event)
	{
		m_Tool = Tool::None;

		QObject* parentObj = parent();
		QWidget* parentWidget = dynamic_cast<QWidget*>(parentObj);
		BOOST_ASSERT(parentWidget);

		auto bbox = GetSelectionBoundingBox();
		Vector2D center;
		double radius;
		bbox.GetCircumcircle(center, radius);
		QPointF centerPixel = m_Renderer->Deproject(center);
		double radiusPixel = radius * m_Renderer->GetViewportZoom();
		QPointF delta = event->position() - centerPixel;

		double distanceSquared = delta.x() * delta.x() + delta.y() * delta.y();
		if (fabs(sqrt(distanceSquared) - radiusPixel) < RotatorPixelDistance)
		{
			m_Tool = Tool::Rotate;
			parentWidget->setCursor(Qt::CrossCursor);
			return;
		}

		Vector2D cursorProjected = m_Renderer->Project(event->position());
		if (bbox.Contains(cursorProjected))
		{
			m_Tool = Tool::Drag;
			parentWidget->setCursor(Qt::SizeAllCursor);
			return;
		}

		parentWidget->unsetCursor();
	}

	AxisAlignedBoundingBox2D ImageDraftEditor::GetSelectionBoundingBox() const
	{
		const auto& selectedDrafts = m_SelectionManager->GetSelectedDrafts();
		BOOST_ASSERT(selectedDrafts.size() > 0);

		bool minMaxValid = false;
		Vector2D min;
		Vector2D max;
		for (const auto& draft : m_SelectionManager->GetSelectedDrafts())
		{
			auto bbox = draft->GetAxisAlignedBoundingBox();
			Vector2D boxMin = bbox.GetMin();
			Vector2D boxMax = bbox.GetMax();
			if (!minMaxValid)
			{
				min = boxMin;
				max = boxMax;
				minMaxValid = true;
				continue;
			}

			if (boxMin.GetX() < min.GetX())
			{
				min.SetX(boxMin.GetX());
			}
			if (boxMin.GetY() < min.GetY())
			{
				min.SetY(boxMin.GetY());
			}
			if (boxMax.GetX() > max.GetX())
			{
				max.SetX(boxMax.GetX());
			}
			if (boxMax.GetY() > max.GetY())
			{
				max.SetY(boxMax.GetY());
			}
		}
		AxisAlignedBoundingBox2D bbox{ min, max };
		return bbox;
	}

	void ImageDraftEditor::Drag(QMouseEvent* event)
	{
		Vector2D first = m_Renderer->Project(m_PreviousPoint);
		Vector2D second = m_Renderer->Project(event->position());
		Vector2D delta = second - first;

		for (const auto& draft : m_SelectionManager->GetSelectedDrafts())
		{
			Transform2D transform = draft->GetTransform();
			transform.Translation += delta;
			draft->SetTransform(transform);
		}
	}

	void ImageDraftEditor::Rotate(QMouseEvent* event)
	{
		AxisAlignedBoundingBox2D bbox = GetSelectionBoundingBox();

		Vector2D center = bbox.GetCenter();
		Vector2D first = m_Renderer->Project(m_PreviousPoint);
		Vector2D second = m_Renderer->Project(event->position());

		Vector2D v1 = first - center;
		Vector2D v2 = second - center;
		
		double angle = Vector2D::GetAngleBetweenPoints(v1, v2);
		GetLogger()->info("Angle: {}", angle * 180.0 / M_PI);
	}

	void ImageDraftEditor::Scale(QMouseEvent* event)
	{
	}

}
