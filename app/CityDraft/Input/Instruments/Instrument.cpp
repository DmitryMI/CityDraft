#include "Instrument.h"
#include "CityDraft/UI/Rendering/IRenderer.h"

namespace CityDraft::Input::Instruments
{
	Instrument::Instrument(CityDraft::Scene* scene, IKeyBindingProvider* keyBindingProvider, CityDraft::UI::Rendering::IRenderer* renderer, QUndoStack* undoStack, QObject* parent) :
		m_Scene(scene),
		m_KeyBindingProvider(keyBindingProvider),
		m_Renderer(renderer),
		m_UndoStack(undoStack),
		QObject(parent)
	{
		BOOST_ASSERT(scene);
		BOOST_ASSERT(m_KeyBindingProvider);
		BOOST_ASSERT(m_Renderer);
		BOOST_ASSERT(m_UndoStack);
	}

	Instrument::~Instrument()
	{
		
	}

	void Instrument::OnPaint()
	{
	}

	EventChainAction Instrument::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		return EventChainAction::Next;
	}

	EventChainAction Instrument::OnRendererMouseMove(QMouseEvent* event)
	{
		return EventChainAction::Next;
	}

	void Instrument::SetRenderer(CityDraft::UI::Rendering::IRenderer* renderer)
	{
		BOOST_ASSERT(renderer);
		m_Renderer = renderer;
	}

	CityDraft::UI::Rendering::IRenderer* Instrument::GetRenderer() const
	{
		return m_Renderer;
	}

	void Instrument::SetActive(bool active)
	{
		if (active == m_IsActive)
		{
			return;
		}

		m_IsActive = active;
		if (active)
		{
			GetLogger()->info("Activated");
		}
		else
		{
			GetLogger()->info("Deactivated");
		}
	}

	bool Instrument::IsActive() const
	{
		return m_IsActive;
	}

}
