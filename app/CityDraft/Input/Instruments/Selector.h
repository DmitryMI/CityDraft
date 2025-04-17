#pragma once

#include "Instrument.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"

namespace CityDraft::Input::Instruments
{
	class Selector : public Instrument
	{
	public:
		Selector(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QObject* parent = nullptr);
		virtual ~Selector() override;
		inline QString GetName() const override
		{
			return "Selecting";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;

		AxisAlignedBoundingBox2D GetProjectedSelectionBox() const;

	protected:
		QPointF m_FirstMousePosition;
		QPointF m_LastMousePosition;

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("Selector"); };
	};
}
