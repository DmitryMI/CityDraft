#include "Panner.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/UI/Rendering/IRenderer.h"

namespace CityDraft::Input::Instruments
{
	Panner::Panner(const Dependencies& dependencies):
		Instrument(dependencies)
	{
		GetLogger()->debug("Created");
	}

	Panner::~Panner()
	{
		GetLogger()->debug("Destroyed");
	}

	EventChainAction Panner::OnRendererMouseButton(QMouseEvent* event, bool pressed)
	{
		BOOST_ASSERT(IsActive());

		BOOST_ASSERT(m_KeyBindingProvider);

		if (event->button() != m_KeyBindingProvider->GetMouseViewportPanningButton())
		{
			return EventChainAction::Next;
		}

		if (pressed)
		{
			m_LastMousePosition = event->position();
			m_PanPressed = true;
		}
		else
		{
			m_PanPressed = false;
		}
		return EventChainAction::Stop;
	}

	EventChainAction Panner::OnRendererMouseMove(QMouseEvent* event)
	{
		BOOST_ASSERT(IsActive());

		if (!m_PanPressed)
		{
			return EventChainAction::Next;
		}

		QPointF currentPos = event->position();
		QPointF delta = currentPos - m_LastMousePosition;
		m_LastMousePosition = currentPos;

		Vector2D viewportCenter = m_Renderer->GetViewportCenter();
		double viewportZoom = m_Renderer->GetViewportZoom();

		Vector2D deltaScaled = Vector2D(delta.x(), delta.y()) / viewportZoom;
		viewportCenter -= deltaScaled;
		m_Renderer->SetViewportTransform(viewportCenter, viewportZoom);
		m_Renderer->Repaint();

		return EventChainAction::Stop;
	}

	EventChainAction Panner::OnRendererMouseWheel(QWheelEvent* event)
	{
		auto zoom = m_Renderer->GetViewportZoom();
		int delta = event->angleDelta().y();
		zoom += 0.0001 * delta;
		if (zoom == 0)
		{
			zoom = 0.0001;
		}
		GetLogger()->trace("Zoom changed to {}", zoom);
		m_Renderer->SetViewportTransform(m_Renderer->GetViewportCenter(), zoom);
		m_Renderer->Repaint();
		return EventChainAction::Stop;
	}

	void Panner::QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions)
	{
		ToolDescryptor panTool;
		panTool.MouseButton = m_KeyBindingProvider->GetMouseViewportPanningButton();
		if (!toolDescriptions.contains(panTool))
		{
			QString msg = "pan viewport";
			toolDescriptions[panTool] = msg;
		}
	}
}
