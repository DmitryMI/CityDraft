#pragma once

#include "Instrument.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"

namespace CityDraft::Input::Instruments
{
	class ImageDraftEditor : public Instrument
	{
	public:
		ImageDraftEditor(const InstrumentDependencies& dependencies);
		virtual ~ImageDraftEditor() override;
		inline QString GetName() const override
		{
			return "Image Draft Edit";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;


	protected:

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("ImageDraftEditor"); };
	};
}
