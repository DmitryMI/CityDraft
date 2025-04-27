//
// Created by mariiakorolevaa on 23.04.2025.
//

#pragma once

#include <QLabel>
#include <memory>
#include "ImageSelectionWidget.h"
#include "CityDraft/Layer.h"

namespace CityDraft::UI
{

	class LayerItem : public QWidget
	{
		Q_OBJECT

	public:
		LayerItem(CityDraft::Layer* layer, QWidget* parent = nullptr);

		void setVisibleState(bool visible);
		bool eventFilter(QObject* obj, QEvent* event) override;

		inline CityDraft::Layer* GetLayer() const
		{
			return m_Layer;
		}

	signals:
		void removeLayer(CityDraft::Layer* layer);
		void layerRenamed(const QString& oldName, const QString& newName);

	private slots:
		void onToggleVisibility();

	private:
		QLabel* m_label;
		QPushButton* m_eyeButton;
		QPushButton* m_removeButton;
		CityDraft::Layer* m_Layer;

		void updateIcon() const;
	};

}