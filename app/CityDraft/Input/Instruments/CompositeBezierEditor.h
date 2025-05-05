#pragma once

#include "Instrument.h"
#include "CityDraft/Curves/CompositeBezierCurve.h"
#include "CityDraft/Drafts/Curve.h"
#include <vector>

namespace CityDraft::Input::Instruments
{
	class CompositeBezierEditor: public Instrument
	{
	public:

		constexpr static int Priority = 10;
		constexpr static int CurveAnchorPointSize = 5;
		constexpr static int CurveHandlePointSize = 3;

		CompositeBezierEditor(const Dependencies& dependencies);
		virtual ~CompositeBezierEditor();

		void OnActiveFlagChanged() override;

		int GetPriority() const override
		{
			return Priority;
		}

		inline QString GetName() const override
		{
			return "Composite Bezier Edit";
		}

		SelectionResponse GetSelectionResponse(const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& selectedDrafts) const override;

		void OnPaint() override;
		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;
		EventChainAction OnRendererMouseWheel(QWheelEvent* event) override;

		void QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions) override;
		bool HasActiveTool() const override;

	protected:
		inline std::shared_ptr<spdlog::logger> GetLogger() override
		{
			return CityDraft::Logging::LogManager::CreateLogger("CompositeBezierEditor");
		}

	private:
		bool m_DragActive = false;
		std::shared_ptr<CityDraft::Drafts::Curve> m_ActiveDraft = nullptr;
		CityDraft::Curves::CompositeBezierCurve* m_ActiveCurve = nullptr;
		bool m_HasActiveAnchor = false;
		size_t m_ActiveAnchorIndex = 0;
		size_t m_ActiveAnchorComponentIndex = 0;
		boost::signals2::connection m_DraftsSelectedConnection;
		boost::signals2::connection m_DraftsDeselectedConnection;


		QPointF m_FirstPoint;
		QPointF m_PreviousPoint;

		void PaintAnchors(CityDraft::Curves::CompositeBezierCurve* curve);

		bool DetectAnchorComponentUnderCursor(
			QMouseEvent* event,
			size_t& outAnchorIndex,
			size_t& outAnchorComponentIndex
			);

		void Drag(QMouseEvent* event);
		void DragHandles(const Vector2D& delta, Vector2D& handle, Vector2D& oppositeHandle);
		void InsertAnchor(QMouseEvent* event);

		void OnDraftsSelected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& selectedDrafts);
		void OnDraftsDeselected(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& deselectedDrafts);

	};
}
