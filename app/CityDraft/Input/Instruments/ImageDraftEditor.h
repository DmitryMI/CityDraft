#pragma once

#include "Instrument.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Drafts/Draft.h"

namespace CityDraft::Input::Instruments
{
	class ImageDraftEditor : public Instrument
	{
	public:
		enum class Tool
		{
			None,
			Drag,
			Rotate,
			Scale
		};

		constexpr static double RotatorPixelDistance = 10;

		ImageDraftEditor(const Dependencies& dependencies);
		virtual ~ImageDraftEditor() override;
		inline QString GetName() const override
		{
			return "Image Draft Edit";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;

		void OnPaint() override;
	protected:

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("ImageDraftEditor"); };
		void OnActiveFlagChanged() override;
	private:
		Tool m_Tool = Tool::None;
		bool m_ToolInUse = false;
		QPointF m_FirstPoint;
		QPointF m_PreviousPoint;

		void PaintRotatorCircle();
		void DetectTransformationTool(QMouseEvent* event);
		AxisAlignedBoundingBox2D GetSelectionBoundingBox() const;
		double GetRotationDelta(const QPointF& point1, const QPointF& point2, const Vector2D& center);

		void Drag(QMouseEvent* event);
		void Rotate(QMouseEvent* event);
		void Scale(QMouseEvent* event);
	};
}
