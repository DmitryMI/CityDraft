#pragma once

#include "CityDraft/Scene.h"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Transform2D.h"
#include <boost/signals2.hpp>
#include <QMouseEvent>

namespace CityDraft::UI::Rendering
{
	class IRenderer
	{
	public:
		using MouseButtonSignal = boost::signals2::signal<void(QMouseEvent*, bool)>;
		using MouseMovedSignal = boost::signals2::signal<void(QMouseEvent*)>;

		virtual std::shared_ptr<CityDraft::Scene> GetScene() const = 0;
		virtual void Paint(CityDraft::Assets::Asset* asset, const Transform2D& transform) = 0;

		virtual const Vector2D GetViewportCenter() const = 0;
		virtual double GetViewportZoom() const = 0;
		virtual void SetViewportTransform(const Vector2D& center, double zoom) = 0;
		virtual void Repaint() = 0;
	};
}
