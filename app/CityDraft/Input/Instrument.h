#pragma once

#include <QMouseEvent.h>
#include <memory>


namespace CityDraft::Input
{
	class IKeyBindingProvider;

	class Instrument
	{
	public:
		virtual void OnPaint() = 0;
		virtual void OnMousePress(QMouseEvent* event) = 0;
		virtual void OnMouseRelease(QMouseEvent* event) = 0;
		virtual void OnMouseMove(QMouseEvent* event, const QPointF& oldPosition) = 0;

	protected:
		std::shared_ptr<IKeyBindingProvider> m_KeyBindingProvider;
	};
}
