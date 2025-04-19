#pragma once

#include "CityDraft/Assets/SkiaAssetManager.h"
#include "CityDraft/Input/Factory.h"
#include "CityDraft/Scene.h"
#include "Rendering/SkiaWidget.h"
#include "spdlog/spdlog.h"
#include "ui_MainWindow.h"
#include <boost/signals2.hpp>
#include <list>
#include <qlabel.h>
#include <QtWidgets/QMainWindow>
#include "CityDraft/Input/Instruments/Instrument.h"
#include "CityDraft/Input/Instruments/Selector.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include <set>
#include <spdlog/spdlog.h>
#include <QUndoStack>
#include <QMenu>
#include "CityDraft/UI/Colors/IColorsProvider.h"
#include "CityDraft/Input/ISelectionManager.h"

namespace CityDraft::UI
{
    class MainWindow : public QMainWindow, public CityDraft::Input::ISelectionManager
    {
        Q_OBJECT

    public:
        MainWindow(const QString& assetsRoot, QWidget* parent = nullptr);
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
		QLabel* m_CursorProjectedPosition = nullptr;
		QLabel* m_ActiveInstrumentsLabel = nullptr;
		QMenu* m_EditMenu;

		// CityDraft Objects
		std::shared_ptr<CityDraft::Assets::SkiaAssetManager> m_AssetManager;
		std::shared_ptr<CityDraft::Scene> m_Scene;

		// Config
		QString m_AssetsRootDirectory;
		std::shared_ptr<CityDraft::UI::Colors::IColorsProvider> m_ColorsProvider;

		// Input
		std::shared_ptr<CityDraft::Input::IKeyBindingProvider> m_KeyBindingProvider;
		std::list<CityDraft::Input::Instruments::Instrument*> m_InactiveInstruments;
		std::list<CityDraft::Input::Instruments::Instrument*> m_ActiveInstruments;
		std::set<std::shared_ptr<CityDraft::Drafts::Draft>> m_SelectedDrafts;

		void CreateUndoRedoStack(QMenu* menu);
		void CreateRenderingWidget();
		void CreateAssetManager(const QString& assetsRoot);
		void CreateStatusBar();
		void CreateMenuBar();

		void CreateInstruments();
		void UpdateActiveInstrumentsLabel();
		void ProcessInstrumentsMouseMoveEvent(QMouseEvent* event);

		template<typename T>
		T* FindInstrument()
		{
			auto iter = std::find_if(m_InactiveInstruments.begin(), m_InactiveInstruments.end(), [](auto* instrument) {return dynamic_cast<T*>(instrument) != nullptr; });
			if (iter != m_InactiveInstruments.end())
			{
				return dynamic_cast<T*>(*iter);
			}

			iter = std::find_if(m_ActiveInstruments.begin(), m_ActiveInstruments.end(), [](auto* instrument) {return dynamic_cast<T*>(instrument) != nullptr; });
			if (iter != m_ActiveInstruments.end())
			{
				return dynamic_cast<T*>(*iter);
			}

			BOOST_ASSERT(false);
			return nullptr;
		}

		template<typename T>
		T* TransitInstrument(std::list<CityDraft::Input::Instruments::Instrument*>& from, std::list<CityDraft::Input::Instruments::Instrument*>& to)
		{
			auto iter = std::find_if(from.begin(), from.end(), [](auto* instrument) {return dynamic_cast<T*>(instrument) != nullptr; });
			BOOST_ASSERT(iter != from.end());
			T* instrument = dynamic_cast<T*>(*iter);
			BOOST_ASSERT(instrument);
			from.erase(iter);
			to.push_back(instrument);
			
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

		
		// Selection
		void StartSelection(QMouseEvent* event, CityDraft::Input::Instruments::Selector* selector);

		// Undo-Redo
		QUndoStack* m_UndoStack;

	private slots:
		void OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget);
		void OnGraphicsPainting(UI::Rendering::SkiaWidget* widget);
		void OnRenderingWidgetMouseButtonEvent(QMouseEvent* event, bool pressed);
		void OnRenderingWidgetMouseMoveEvent(QMouseEvent* event);
		void OnInstrumentFinished(CityDraft::Input::Instruments::Instrument* instrument, CityDraft::Input::Instruments::FinishStatus status);
    };

}
