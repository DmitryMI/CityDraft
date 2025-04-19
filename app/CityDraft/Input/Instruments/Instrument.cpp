#include "Instrument.h"
#include "CityDraft/UI/Rendering/IRenderer.h"

namespace CityDraft::Input::Instruments
{
	Instrument::Instrument(const InstrumentDependencies& dependencies) :
		m_Scene(dependencies.Scene),
		m_KeyBindingProvider(dependencies.KeyBindingProvider),
		m_ColorsProvider(dependencies.ColorsProvider),
		m_Renderer(dependencies.Renderer),
		m_UndoStack(dependencies.UndoStack),
		QObject(dependencies.Parent)
	{
		BOOST_ASSERT(m_Scene);
		BOOST_ASSERT(m_KeyBindingProvider);
		BOOST_ASSERT(m_ColorsProvider);
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
		OnActiveFlagChanged();
	}

	bool Instrument::IsActive() const
	{
		return m_IsActive;
	}

}
