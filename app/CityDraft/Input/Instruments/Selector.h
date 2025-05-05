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
		constexpr static int MultiSelectMouseMoveThreshold = 5;

		Selector(const Dependencies& dependencies);
		virtual ~Selector() override;
		int GetPriority() const override{ return Priority; }

		inline QString GetName() const override
		{
			return "Selecting";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;
		void OnPaint() override;

		AxisAlignedBoundingBox2D GetProjectedSelectionBox() const;
		size_t GetSelectedDrafts(std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts) const;

		void QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions) override;
		bool HasActiveTool() const override;

	protected:
		QPointF m_FirstMousePosition;
		QPointF m_LastMousePosition;
		bool m_SelectionPressed = false;
		bool m_IsMultiSelection = false;
		CityDraft::Drafts::Draft* m_DraftUnderCursor = nullptr;

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("Selector"); };
		void FinishSelection(QMouseEvent* event);
		
	private:
		boost::signals2::connection m_DraftRemovedConnection;

		void OnDraftRemoved(CityDraft::Drafts::Draft* draft);
	};
}
