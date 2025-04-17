#pragma once

#include <QMouseEvent>
#include <QString>
#include <memory>
#include <boost/signals2.hpp>
#include <qobject.h>

namespace CityDraft::UI::Rendering
{
	class IRenderer;
}

namespace CityDraft::Input
{
	class IKeyBindingProvider;
}


namespace CityDraft::Input::Instruments
{
	
	enum class EventChainAction
	{
		Next,
		Stop
	};

	class Instrument : public QObject
	{
		Q_OBJECT;
	public:
		Instrument(IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QObject* parent = nullptr);

		virtual QString GetName() const = 0;
		virtual void OnPaint();
		virtual EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed);
		virtual EventChainAction OnRendererMouseMove(QMouseEvent* event);

		void SetRenderer(CityDraft::UI::Rendering::IRenderer* renderer);
		CityDraft::UI::Rendering::IRenderer* GetRenderer() const;

	signals:
		void Finished(Instrument* instrument);

	protected:
		IKeyBindingProvider* m_KeyBindingProvider;
		CityDraft::UI::Rendering::IRenderer* m_Renderer;
	};
}
