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
		int GetPriority() const override{ return Priority; }

		inline QString GetName() const override
		{
			return "Panning";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;
		EventChainAction OnRendererMouseWheel(QWheelEvent* event) override;

		void QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions) override;
		bool HasActiveTool() const override;

	protected:
		QPointF m_LastMousePosition;
		bool m_PanPressed = false;

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("Panner"); } ;
	};
}
