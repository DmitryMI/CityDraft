#pragma once

#include "Painter.h"
#include "CityDraft/Transform2D.h"
#include "CityDraft/Assets/Asset.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Asset : public Painter
	{
	public:
		inline Asset(const CityDraft::Transform2D& transform) : m_Transform(transform) {};

		virtual void Paint(CityDraft::UI::Rendering::SkiaWidget* renderer, SkCanvas* canvas) = 0;

	protected: 
		CityDraft::Transform2D m_Transform;

	};
}
