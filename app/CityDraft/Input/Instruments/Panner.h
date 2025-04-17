#pragma once

#include "Instrument.h"

namespace CityDraft::Input::Instruments
{
	class Panner : public Instrument
	{
	public:
		Panner(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QObject* parent = nullptr);
		virtual ~Panner() override;
		inline QString GetName() const override
		{
			return "Panning";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;

	protected:
		QPointF m_LastMousePosition;

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("Selector"); } ;
	};
}
