#pragma once

#include "Instrument.h"

namespace CityDraft::Input::Instruments
{
	class Panner : public Instrument
	{
	public:
		constexpr static int Priority = -90;

		Panner(const Dependencies& dependencies);
		virtual ~Panner() override;
		virtual int GetPriority() const { return Priority; }

		inline QString GetName() const override
		{
			return "Panning";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;

	protected:
		QPointF m_LastMousePosition;

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("Panner"); } ;
	};
}
