#pragma once

#include "Instrument.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Drafts/Draft.h"

namespace CityDraft::Input::Instruments
{
	class Selector : public Instrument
	{
	public:
		constexpr static int Priority = -100;

		Selector(const Dependencies& dependencies);
		virtual ~Selector() override;
		virtual int GetPriority() const { return Priority; }

		inline QString GetName() const override
		{
			return "Selecting";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;
		void OnPaint() override;

		AxisAlignedBoundingBox2D GetProjectedSelectionBox() const;
		size_t GetSelectedDrafts(std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts) const;

	protected:
		QPointF m_FirstMousePosition;
		QPointF m_LastMousePosition;

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("Selector"); };
	};
}
