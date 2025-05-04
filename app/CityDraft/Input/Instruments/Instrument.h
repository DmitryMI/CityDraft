#pragma once

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QString>
#include <memory>
#include <boost/signals2.hpp>
#include <qobject.h>
#include <spdlog/spdlog.h>
#include <QUndoStack>
#include "CityDraft/Logging/LogManager.h"
#include <map>
#include <set>
#include "CityDraft/Drafts/Draft.h"

namespace CityDraft::UI::Rendering
{
	class IRenderer;
}

namespace CityDraft::UI::Colors
{
	class IColorsProvider;
}

namespace CityDraft::Input
{
	class IKeyBindingProvider;
	class ISelectionManager;
}

namespace CityDraft
{
	class Scene;
}

namespace CityDraft::Input::Instruments
{
	enum class EventChainAction
	{
		Next,
		Stop
	};

	enum class SelectionResponse
	{
		KeepCurrentState,
		WantsToActivate,
		WantsToDeactivate
	};

	enum class FinishStatus
	{
		Ok,
		Canceled,
		Error
	};

	struct Dependencies
	{
		CityDraft::Scene* Scene;
		CityDraft::Input::ISelectionManager* SelectionManager;
		CityDraft::Input::IKeyBindingProvider* KeyBindingProvider;
		CityDraft::UI::Colors::IColorsProvider* ColorsProvider;
		CityDraft::UI::Rendering::IRenderer* Renderer;
		QUndoStack* UndoStack;
		QObject* Parent;
	};

	struct ToolDescryptor
	{
		std::optional<Qt::MouseButton> MouseButton = std::nullopt;
		std::optional<Qt::KeyboardModifier> Modifier = std::nullopt;
		std::optional<Qt::Key> Key = std::nullopt;
		bool Wheel = false;

		bool operator<(const ToolDescryptor& other) const
		{
			return std::tie(MouseButton, Modifier, Key, Wheel) <
				std::tie(other.MouseButton, other.Modifier, other.Key, other.Wheel);
		}
	};

	class Instrument : public QObject
	{
		Q_OBJECT;
	public:

		Instrument(const Dependencies& dependencies);
		virtual ~Instrument();
		virtual QString GetName() const = 0;
		virtual void OnPaint();
		virtual EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed);
		virtual EventChainAction OnRendererMouseMove(QMouseEvent* event);
		virtual EventChainAction OnRendererMouseWheel(QWheelEvent* event);
		virtual EventChainAction OnRendererKey(QKeyEvent* event);
		virtual int GetPriority() const { return 0; }

		void SetRenderer(CityDraft::UI::Rendering::IRenderer* renderer);
		CityDraft::UI::Rendering::IRenderer* GetRenderer() const;
		void SetActive(bool active);
		bool IsActive() const;

		virtual void QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions) = 0;

		virtual SelectionResponse GetSelectionResponse(const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& selectedDrafts) const
		{
			return SelectionResponse::KeepCurrentState;
		}

	signals:
		void Finished(Instrument* instrument, FinishStatus status = FinishStatus::Ok);

	protected:
		CityDraft::Scene* m_Scene = nullptr;
		CityDraft::Input::ISelectionManager* m_SelectionManager;
		IKeyBindingProvider* m_KeyBindingProvider = nullptr;
		CityDraft::UI::Colors::IColorsProvider* m_ColorsProvider;
		CityDraft::UI::Rendering::IRenderer* m_Renderer = nullptr;
		QUndoStack* m_UndoStack = nullptr;
		bool m_IsActive = false;

		virtual inline std::shared_ptr<spdlog::logger> GetLogger()
		{
			return CityDraft::Logging::LogManager::CreateLogger("Instrument");
		};

		virtual inline void OnActiveFlagChanged() {};
	};

	struct Comparator
	{
		bool operator()(const Instrument* a, const Instrument* b) const
		{
			if (a->GetPriority() != b->GetPriority())
			{
				return a->GetPriority() > b->GetPriority();
			}
			BOOST_ASSERT(a->GetName() != b->GetName());
			return a->GetName() < b->GetName();
		}
	};
}
