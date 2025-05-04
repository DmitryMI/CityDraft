#pragma once

#include "Instrument.h"
#include "CityDraft/Curves/CompositeBezierCurve.h"
#include "CityDraft/Drafts/Curve.h"

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
		int GetPriority() const override
		{
			return Priority;
		}

		inline QString GetName() const override
		{
			return "Composite Bezier Edit";
		}

		SelectionResponse GetSelectionResponse(const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& selectedDrafts) const override
		{
			for(const auto& draft : selectedDrafts)
			{
				auto curveDraft = dynamic_pointer_cast<CityDraft::Drafts::Curve>(draft);
				if(!curveDraft)
				{
					continue;
				}
				auto bezier = dynamic_pointer_cast<CityDraft::Curves::CompositeBezierCurve>(curveDraft->GetCurve());
				if(bezier)
				{
					return SelectionResponse::WantsToActivate;
				}
			}
			return SelectionResponse::WantsToDeactivate;
		}

		void OnPaint() override;
		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;
		EventChainAction OnRendererMouseWheel(QWheelEvent* event) override;

		void QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions) override;

	protected:
		inline std::shared_ptr<spdlog::logger> GetLogger() override
		{
			return CityDraft::Logging::LogManager::CreateLogger("CompositeBezierEditor");
		}

	private:
		bool m_DragActive = false;
		CityDraft::Drafts::Curve* m_ActiveDraft = nullptr;
		CityDraft::Curves::CompositeBezierCurve* m_ActiveCurve = nullptr;
		size_t m_ActiveAnchorIndex = 0;
		size_t m_ActiveAnchorComponentIndex = 0;
		QPointF m_FirstPoint;
		QPointF m_PreviousPoint;

		void PaintAnchors(CityDraft::Curves::CompositeBezierCurve* curve);

		bool DetectAnchorComponentUnderCursor(
			QMouseEvent* event,
			CityDraft::Drafts::Curve*& outDraft,
			CityDraft::Curves::CompositeBezierCurve*& outCurve, 
			size_t& outAnchorIndex,
			size_t& outAnchorComponentIndex
			);

		bool DetectAnchorComponentInCurveUnderCursor(
			QMouseEvent* event,
			Vector2D& projected,
			CityDraft::Curves::CompositeBezierCurve* curve,
			size_t& outAnchorIndex,
			size_t& outAnchorComponentIndex
		);

		void Drag(QMouseEvent* event);
		void DragHandles(const Vector2D& delta, Vector2D& handle, Vector2D& oppositeHandle);
		void InsertAnchor(QMouseEvent* event);
	};
}
