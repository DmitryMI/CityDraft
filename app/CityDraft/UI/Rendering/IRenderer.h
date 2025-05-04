#pragma once

#include "CityDraft/Scene.h"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Transform2D.h"
#include <boost/signals2.hpp>
#include <QMouseEvent>
#include "CityDraft/LinearColor.h"

namespace CityDraft::UI::Rendering
{
	class IRenderer
	{
	public:
		using MouseButtonSignal = boost::signals2::signal<void(QMouseEvent*, bool)>;
		using MouseMovedSignal = boost::signals2::signal<void(QMouseEvent*)>;

		virtual std::shared_ptr<CityDraft::Scene> GetScene() const = 0;

		// Drawing
		virtual void Paint(CityDraft::Assets::Asset* asset, const Transform2D& transform) = 0;
		virtual void PaintRectViewportSpace(const QPointF& pixelMin, const QPointF& pixelMax, const LinearColorF& color, double thickness) = 0;
		virtual void PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& outlineColor, double outlineThickness) = 0;
		virtual void PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& outlineColor, double outlineThickness, const LinearColorF& fillColor) = 0;
		virtual void PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& fillColor) = 0;
		virtual void PaintCircle(const Vector2D& pos, double radius, const LinearColorF& color, double thickness) = 0;
		virtual void PaintLine(const Vector2D& from, const Vector2D& to, const LinearColorF& color, double thickness) = 0;
		virtual void Repaint() = 0;

		// Model
		virtual Vector2D Project(const QPointF& pixelCoord) const = 0;
		virtual QPointF Deproject(const Vector2D& pixelCoord) const = 0;
		virtual const Vector2D GetViewportCenter() const = 0;
		virtual double GetViewportZoom() const = 0;
		virtual void SetViewportTransform(const Vector2D& center, double zoom) = 0;
	};
}
