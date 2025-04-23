//
// Created by mariiakorolevaa on 19.04.2025.
//

#pragma once

#include <QPushButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <complex.h>
#include <qgridlayout.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/UI/FlowLayout.h"

namespace CityDraft::Assets
{
	class Asset;
}

namespace CityDraft::UI
{

	class ImageSelectionWidget final : public QWidget
	{
		Q_OBJECT

	public:
		constexpr static int ImageSize = 64;

		explicit ImageSelectionWidget(CityDraft::Assets::AssetManager* assetManager, QWidget* parent = nullptr);
		virtual ~ImageSelectionWidget();

		void loadImagesFromAssets();

		void resizeEvent(QResizeEvent* event) override;
	signals:
		void imageSelected(const QString& imagePath);
		void imageVariantSelected(std::shared_ptr<CityDraft::Assets::Image>);
		void imageGroupSelected(std::shared_ptr<CityDraft::Assets::ImageVariantGroup> group);

	private:
		QGridLayout* m_ScrollableLayout;
		std::vector<QPushButton*> m_ImageButtons;
		CityDraft::Assets::AssetManager* m_AssetManager;
		QTimer* m_ResizeTimer;

		void UpdateButtonsPlacement();

	private slots:
		void OnResizeFinished();

	};

}