#include "ImageDraftEditor.h"
#include "CityDraft/Input/ISelectionManager.h"
#include "CityDraft/UI/Rendering/IRenderer.h"
#include "CityDraft/UI/Colors/IColorsProvider.h"

namespace CityDraft::Input::Instruments
{
    ImageDraftEditor::ImageDraftEditor(const InstrumentDependencies& dependencies):
		Instrument(dependencies)
    {
    }

	EventChainAction ImageDraftEditor::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		BOOST_ASSERT(IsActive());

		return EventChainAction();
	}

	EventChainAction ImageDraftEditor::OnRendererMouseMove(QMouseEvent* event)
	{
		BOOST_ASSERT(IsActive());



		return EventChainAction();
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

	void ImageDraftEditor::PaintRotatorCircle()
	{
		AxisAlignedBoundingBox2D bbox = GetSelectionBoundingBox();
		double radius = 0.5f * sqrt(bbox.GetSize().GetX() * bbox.GetSize().GetX() + bbox.GetSize().GetY() * bbox.GetSize().GetY());
		m_Renderer->PaintRect(bbox.GetMin(), bbox.GetMax(), m_ColorsProvider->GetDraftScaleBoxColor(), 2.0 / m_Renderer->GetViewportZoom());
		m_Renderer->PaintCircle(bbox.GetCenter(), radius, m_ColorsProvider->GetDraftRotationCircleColor(), 2.0 / m_Renderer->GetViewportZoom());
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

}
