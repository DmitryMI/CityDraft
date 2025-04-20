#include "ImageDraftEditor.h"
#include "CityDraft/Input/ISelectionManager.h"
#include "CityDraft/UI/Colors/IColorsProvider.h"
#include <QWidget>
#include "CityDraft/Input/IKeyBindingProvider.h"

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
				m_OldTransforms.clear();

				for (const auto& draft : m_SelectionManager->GetSelectedDrafts())
				{
					m_OldTransforms[draft] = draft->GetTransform();
				}

				return EventChainAction::Stop;
			}
		}
		else
		{
			Finalize();
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

		AxisAlignedBoundingBox2D bbox = GetSelectionBoundingBox();

		if (!m_ToolInUse)
		{
			DetectTransformationTool(bbox, event);
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
			Rotate(event, bbox);
			break;
		case Tool::Scale:
			Scale(event, bbox);
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

		AxisAlignedBoundingBox2D bbox = GetSelectionBoundingBox();

		PaintRotatorCircle(bbox);
		PaintScalingRects(bbox);
	}

	void ImageDraftEditor::OnActiveFlagChanged()
	{
		QObject* parentObj = parent();
		QWidget* parentWidget = dynamic_cast<QWidget*>(parentObj);
		BOOST_ASSERT(parentWidget);
		parentWidget->unsetCursor();

		if (m_ToolInUse)
		{
			Cancel();
		}

		m_Tool = Tool::None;
		m_ScalingRectIndex = -1;
		m_ToolInUse = false;
		m_OldTransforms.clear();
	}

	void ImageDraftEditor::GetScalingRectsPositions(const AxisAlignedBoundingBox2D& bbox, std::array<Vector2D, 4>& rects)
	{
		Vector2D min = bbox.GetMin();
		Vector2D max = bbox.GetMax();
		rects[0] = { min.GetX(), min.GetY() };
		rects[1] = { min.GetX(), max.GetY() };
		rects[2] = { max.GetX(), max.GetY() };
		rects[3] = { max.GetX(), min.GetY() };
	}

	void ImageDraftEditor::PaintRotatorCircle(const AxisAlignedBoundingBox2D& bbox)
	{
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

	void ImageDraftEditor::PaintScalingRects(const AxisAlignedBoundingBox2D& bbox)
	{
		std::array<Vector2D, 4> rects;
		GetScalingRectsPositions(bbox, rects);
		constexpr double scalingRectSizeHalf = ScalingRectsSize / 2.0;
		double viewportScale = m_Renderer->GetViewportZoom();
		for (size_t i = 0; i < rects.size(); i++)
		{
			Vector2D min = rects[i] - Vector2D{scalingRectSizeHalf, scalingRectSizeHalf} / viewportScale;
			Vector2D max = rects[i] + Vector2D{scalingRectSizeHalf, scalingRectSizeHalf} / viewportScale;
			m_Renderer->PaintRect(min, max, m_ColorsProvider->GetDraftScaleKnobsColor());
		}
	}

	void ImageDraftEditor::DetectTransformationTool(const AxisAlignedBoundingBox2D& bbox, QMouseEvent* event)
	{
		m_Tool = Tool::None;
		m_ScalingRectIndex = -1;

		QObject* parentObj = parent();
		QWidget* parentWidget = dynamic_cast<QWidget*>(parentObj);
		BOOST_ASSERT(parentWidget);

		std::array<Vector2D, 4> rects;
		GetScalingRectsPositions(bbox, rects);
		constexpr double scalingRectSizeHalf = ScalingRectsSize / 2.0;
		double viewportScale = m_Renderer->GetViewportZoom();

		for (size_t i = 0; i < rects.size(); i++)
		{
			QPointF rectPixelCenter = m_Renderer->Deproject(rects[i]);
			QPointF cursorDelta = event->position();
			QPointF min = rectPixelCenter - QPointF(scalingRectSizeHalf, scalingRectSizeHalf);
			QPointF max = rectPixelCenter + QPointF(scalingRectSizeHalf, scalingRectSizeHalf);
			if (
				min.x() <= cursorDelta.x() && cursorDelta.x() <= max.x() &&
				min.y() <= cursorDelta.y() && cursorDelta.y() <= max.y()
				)
			{
				m_ScalingRectIndex = i;
				m_Tool = Tool::Scale;
				if (i == 1 || i == 3)
				{
					parentWidget->setCursor(Qt::SizeBDiagCursor);
				}
				else
				{
					parentWidget->setCursor(Qt::SizeFDiagCursor);
				}
				return;
			}
		}

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

	double ImageDraftEditor::GetRotationDelta(const QPointF& point1, const QPointF& point2, const Vector2D& center)
	{
		Vector2D first = m_Renderer->Project(point1);
		Vector2D second = m_Renderer->Project(point2);

		Vector2D v1 = first - center;
		Vector2D v2 = second - center;

		double angle = Vector2D::GetAngleBetweenPoints(v1, v2);
		return angle;
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

	void ImageDraftEditor::Rotate(QMouseEvent* event, const AxisAlignedBoundingBox2D& bbox)
	{
		double angle = GetRotationDelta(m_PreviousPoint, event->position(), bbox.GetCenter());
		GetLogger()->info("Angle: {}", angle * 180.0 / M_PI);

		for (const auto& draft : m_SelectionManager->GetSelectedDrafts())
		{
			Transform2D transform = draft->GetTransform();
			transform.RotateAroundPoint(angle, bbox.GetCenter());
			draft->SetTransform(transform);
		}
	}

	void ImageDraftEditor::Scale(QMouseEvent* event, const AxisAlignedBoundingBox2D& bbox)
	{
		std::array<Vector2D, 4> rects;
		GetScalingRectsPositions(bbox, rects);

		Vector2D scaleCenter;
		if (event->modifiers().testFlag(m_KeyBindingProvider->GetScaleFromCenterModifier()))
		{
			scaleCenter = bbox.GetCenter();
		}
		else
		{
			scaleCenter = rects[(m_ScalingRectIndex + 2) % 4];
		}

		Vector2D p0 = m_Renderer->Project(m_PreviousPoint);
		Vector2D p1 = m_Renderer->Project(event->position());
		Vector2D initialVec = p0 - scaleCenter;
		Vector2D currentVec = p1 - scaleCenter;
		double originalLength = initialVec.GetSize();
		double currentLength = currentVec.GetSize();
		double scaleFactor = currentLength / originalLength;

		for (const auto& draft : m_SelectionManager->GetSelectedDrafts())
		{
			Transform2D transform = draft->GetTransform();
			transform.ScaleRelativeToPoint(Vector2D{ scaleFactor, scaleFactor}, scaleCenter);
			draft->SetTransform(transform);
		}
	}

	void ImageDraftEditor::Finalize()
	{
		std::vector<TransformChange> changes;
		for (const auto& draftTransformPair : m_OldTransforms)
		{
			TransformChange change{ draftTransformPair.first, draftTransformPair.second, draftTransformPair.first->GetTransform() };
			changes.push_back(change);
		}

		TransformChangeCommand* command = new TransformChangeCommand(changes, m_Renderer);
		m_UndoStack->push(command);
	}

	void ImageDraftEditor::Cancel()
	{
		for (const auto& draftTransformPair : m_OldTransforms)
		{
			draftTransformPair.first->SetTransform(draftTransformPair.second);
		}
	}

}
