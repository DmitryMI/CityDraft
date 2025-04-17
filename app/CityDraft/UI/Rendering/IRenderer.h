#pragma once

#include "CityDraft/Scene.h"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Transform2D.h"

namespace CityDraft::UI::Rendering
{
	class IRenderer
	{
	public:
		virtual std::shared_ptr<CityDraft::Scene> GetScene() const = 0;
		virtual void Paint(CityDraft::Assets::Asset* asset, const Transform2D& transform) = 0;
	};
}
