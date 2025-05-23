//
// Created by mariiakorolevaa on 23.04.2025.
//

#pragma once

#include <QLabel>
#include <memory>
#include <QPushButton>
#include <QUndoStack>
#include "CityDraft/Scene.h"
#include "CityDraft/Layer.h"
#include <boost/signals2/connection.hpp>

namespace CityDraft::UI::Layers
{

	class ItemWidget : public QWidget
	{
		Q_OBJECT

	public:
		ItemWidget(CityDraft::Scene* scene, std::shared_ptr<CityDraft::Layer> layer, QUndoStack* undoStack, QWidget* parent = nullptr);
		~ItemWidget();

		void setVisibleState(bool visible);
		bool eventFilter(QObject* obj, QEvent* event) override;

		inline std::shared_ptr<CityDraft::Layer> GetLayer() const
		{
			return m_Layer;
		}

	private slots:
		void onToggleVisibility();

	private:
		QLabel* m_label;
		QPushButton* m_eyeButton;
		QPushButton* m_removeButton;
		CityDraft::Scene* m_Scene;
		std::shared_ptr<CityDraft::Layer> m_Layer;
		QUndoStack* m_UndoStack;

		boost::signals2::connection m_LayerRenamedConnection;
		boost::signals2::connection m_LayerFlagChangedConnection;

		void OnLayerRenamed(CityDraft::Layer* layer, const std::string& nameOld, const std::string& nameNew);
		void OnLayerFlagChanged(CityDraft::Layer* layer);
		void updateIcon() const;


	};

}