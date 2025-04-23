#pragma once

#include <boost/assert.hpp>
#include <memory>
#include <qevent.h>
#include <qlabel.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qundostack.h>
#include <qwidget.h>
#include <set>
#include <set>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <vector>
#include "CityDraft/Assets/SkiaAssetManager.h"
#include "CityDraft/Drafts/Draft.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/Input/ISelectionManager.h"
#include "CityDraft/Input/Instruments/Instrument.h"
#include "CityDraft/Scene.h"
#include "CityDraft/UI/Colors/IColorsProvider.h"
#include "CityDraft/UI/Rendering/ImageSelectionWidget.h"
#include "CityDraft/UI/Rendering/SkiaWidget.h"
#include "ui_MainWindow.h"

namespace CityDraft::UI
{
	class MainWindow : public QMainWindow, public CityDraft::Input::ISelectionManager
	{
		Q_OBJECT

	public:
		MainWindow(const QString& assetsRoot, const QString& scenePath, QWidget* parent = nullptr);
		virtual ~MainWindow();

		// ISelectionManager
		const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& GetSelectedDrafts() const override;
		void ClearSelectedDrafts() override;
		void AddDraftsToSelection(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>&) override;

	private:
		std::shared_ptr<spdlog::logger> m_Logger;

		// Widgets
		Ui::MainWindow m_Ui;
		UI::Rendering::SkiaWidget* m_RenderingWidget = nullptr;
		ImageSelectionWidget* m_ImageSelectionWidget = nullptr;
		QLabel* m_CursorProjectedPosition = nullptr;
		QLabel* m_ActiveInstrumentsLabel = nullptr;

		// CityDraft Objects
		std::shared_ptr<CityDraft::Assets::SkiaAssetManager> m_AssetManager;
		std::shared_ptr<CityDraft::Scene> m_Scene;

		// Config
		QString m_AssetsRootDirectory;
		QString m_ScenePath;
		std::shared_ptr<CityDraft::UI::Colors::IColorsProvider> m_ColorsProvider;

		// Input
		std::shared_ptr<CityDraft::Input::IKeyBindingProvider> m_KeyBindingProvider;
		std::set<CityDraft::Input::Instruments::Instrument*, CityDraft::Input::Instruments::Comparator> m_InactiveInstruments;
		std::set<CityDraft::Input::Instruments::Instrument*, CityDraft::Input::Instruments::Comparator> m_ActiveInstruments;
		std::set<std::shared_ptr<CityDraft::Drafts::Draft>> m_SelectedDrafts;

		// Undo-Redo
		QUndoStack* m_UndoStack;

		// Initialization helpers
		void CreateUndoRedoStack(QMenu* menu);
		void CreateRenderingWidget();
		void CreateAssetManager(const QString& assetsRoot);
		void CreateStatusBar();
		void CreateInstruments();
		void CreateImageSelectionWidget();
		void LoadImagesToSelectionWidget() const;

		void UpdateActiveInstrumentsLabel();
		void ProcessInstrumentsMouseButtonEvent(QMouseEvent* event, bool pressed);
		void ProcessInstrumentsMouseMoveEvent(QMouseEvent* event);
		void ProcessInstrumentsMouseWheelEvent(QWheelEvent* event);
		void ProcessInstrumentsKeyboardEvent(QKeyEvent* event);

		template<typename T>
		T* FindInstrument()
		{
			auto iterInactive = std::find_if(m_InactiveInstruments.cbegin(), m_InactiveInstruments.cend(), [](auto* instrument) {return dynamic_cast<T*>(instrument) != nullptr; });
			if (iterInactive != m_InactiveInstruments.end())
			{
				return dynamic_cast<T*>(*iterInactive);
			}
			auto iterActive = std::find_if(m_ActiveInstruments.cbegin(), m_ActiveInstruments.cend(), [](auto* instrument) {return dynamic_cast<T*>(instrument) != nullptr; });
			if (iterActive != m_ActiveInstruments.end())
			{
				return dynamic_cast<T*>(*iterActive);
			}

			BOOST_ASSERT(false);
			return nullptr;
		}

		template<typename T, typename ContainerFrom, typename ContainerTo>
		T* TransitInstrument(ContainerFrom& from, ContainerTo& to)
		{
			auto iter = std::find_if(from.begin(), from.end(), [](auto* instrument) {return dynamic_cast<T*>(instrument) != nullptr; });
			BOOST_ASSERT(iter != from.end());
			T* instrument = dynamic_cast<T*>(*iter);
			BOOST_ASSERT(instrument);
			from.erase(iter);
			to.insert(instrument);

			UpdateActiveInstrumentsLabel();
			return instrument;
		}

		template<typename T>
		T* ActivateInstrument()
		{
			T* instrument = TransitInstrument<T>(m_InactiveInstruments, m_ActiveInstruments);
			BOOST_ASSERT(!instrument->IsActive());
			instrument->SetActive(true);
			return instrument;
		}

		template<typename T>
		T* DeactivateInstrument()
		{
			T* instrument = TransitInstrument<T>(m_ActiveInstruments, m_InactiveInstruments);
			BOOST_ASSERT(instrument->IsActive());
			instrument->SetActive(false);
			return instrument;
		}

		void ActivateInstrument(CityDraft::Input::Instruments::Instrument* instrument);
		void DeactivateInstrument(CityDraft::Input::Instruments::Instrument* instrument);

	private slots:
		void OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget);
		void OnGraphicsPainting(UI::Rendering::SkiaWidget* widget);
		void OnRenderingWidgetMouseButtonEvent(QMouseEvent* event, bool pressed);
		void OnRenderingWidgetMouseMoveEvent(QMouseEvent* event);
		void OnRenderingWidgetMouseWheelEvent(QWheelEvent* event);
		void OnRenderingWidgetKeyboardEvent(QKeyEvent* event);
		void OnInstrumentFinished(CityDraft::Input::Instruments::Instrument* instrument, CityDraft::Input::Instruments::FinishStatus status);
		void OnSaveSceneAsClicked();
		void OnOpenSceneClicked();
		void OnNewSceneClicked();
	};

}