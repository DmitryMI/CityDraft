#pragma once

#include "Instrument.h"

namespace CityDraft::Input::Instruments
{
	class Panner : public Instrument
	{
	public:
		Panner(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QObject* parent = nullptr);

		inline QString GetName() const override
		{
			return "Panner";
		}

		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;

	protected:
		QPointF m_LastMousePosition;
	};
}
