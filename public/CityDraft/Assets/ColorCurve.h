#pragma once

#include "Curve.h"

namespace CityDraft::Assets
{
	class AssetManager;

	class ColorCurve: public Curve
	{
	public:

		ColorCurve(AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

	protected:
		void LoadAssetInternal() override;
	};
}
