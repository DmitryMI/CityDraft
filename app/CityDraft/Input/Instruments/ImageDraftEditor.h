#pragma once

#include "Instrument.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Drafts/Draft.h"

namespace CityDraft::Input::Instruments
{
	class ImageDraftEditor : public Instrument
	{
	public:
		constexpr static double RotatorPixelDistance = 10;

		ImageDraftEditor(const InstrumentDependencies& dependencies);
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
		bool m_RotatorActive = false;
		bool m_DragActive = false;
		bool m_ScaleActive = false;

		void PaintRotatorCircle();
		void DetectTransformationTool(QMouseEvent* event);
		AxisAlignedBoundingBox2D GetSelectionBoundingBox() const;
	};
}
