#pragma once

#include "Instrument.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Drafts/Draft.h"
#include <array>
#include <QUndoCommand>
#include "CityDraft/UI/Rendering/IRenderer.h"

namespace CityDraft::Input::Instruments
{
	struct TransformChange
	{
		std::shared_ptr<CityDraft::Drafts::Draft> Draft;
		Transform2D OldTransform;
		Transform2D NewTransform;
	};

	class TransformChangeCommand : public QUndoCommand
	{
	public:
		TransformChangeCommand(
			const std::vector<TransformChange>& changes,
			CityDraft::UI::Rendering::IRenderer* renderer,
			QUndoCommand* parent = nullptr
		):
			m_Changes(changes),
			m_Renderer(renderer),
			QUndoCommand("Transform Change", parent)
		{
		}

		void undo() override
		{
			for (const auto& change : m_Changes)
			{
				change.Draft->SetTransform(change.OldTransform);
			}
			m_Renderer->Repaint();
		}

		void redo() override
		{
			for (const auto& change : m_Changes)
			{
				change.Draft->SetTransform(change.NewTransform);
			}
			m_Renderer->Repaint();
		}

	private:
		std::vector<TransformChange> m_Changes;
		CityDraft::UI::Rendering::IRenderer* m_Renderer = nullptr;
	};

	class DraftTransformer : public Instrument
	{
	public:
		enum class Tool
		{
			None,
			Drag,
			Rotate,
			Scale
		};

		constexpr static double RotatorPixelDistance = 10;
		constexpr static double ScalingRectsSize = 10;
		constexpr static int Priority = 0;

		DraftTransformer(const Dependencies& dependencies);
		virtual ~DraftTransformer() override;
		inline QString GetName() const override
		{
			return "Transform Draft";
		}

		int GetPriority() const override
		{
			return Priority;
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;

		void OnPaint() override;

		void QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions) override;

		inline SelectionResponse GetSelectionResponse(const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& selectedDrafts) const override
		{
			if(selectedDrafts.size() > 0)
			{
				return SelectionResponse::WantsToActivate;
			}
			return SelectionResponse::WantsToDeactivate;
		}

	protected:

		inline std::shared_ptr<spdlog::logger> GetLogger() override { return CityDraft::Logging::LogManager::CreateLogger("DraftTransformer"); };
		void OnActiveFlagChanged() override;
	private:
		Tool m_Tool = Tool::None;
		bool m_ToolInUse = false;
		int m_ScalingRectIndex = -1;
		QPointF m_FirstPoint;
		QPointF m_PreviousPoint;
		std::map<std::shared_ptr<CityDraft::Drafts::Draft>, Transform2D> m_OldTransforms;

		void GetScalingRectsPositions(const AxisAlignedBoundingBox2D& bbox, std::array<Vector2D, 4>& rects);

		void PaintRotatorCircle(const AxisAlignedBoundingBox2D& bbox);
		void PaintScalingRects(const AxisAlignedBoundingBox2D& bbox);

		void DetectTransformationTool(const AxisAlignedBoundingBox2D& bbox, QMouseEvent* event);
		AxisAlignedBoundingBox2D GetSelectionBoundingBox() const;
		Radians GetRotationDelta(const QPointF& point1, const QPointF& point2, const Vector2D& center);

		void Drag(QMouseEvent* event);
		void Rotate(QMouseEvent* event, const AxisAlignedBoundingBox2D& bbox);
		void Scale(QMouseEvent* event, const AxisAlignedBoundingBox2D& bbox);

		void Finalize();
		void Cancel();
	};
}
