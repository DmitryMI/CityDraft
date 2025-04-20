//
// Created by mariiakorolevaa on 19.04.2025.
//

#pragma once

#include <complex.h>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSignalMapper>

#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/UI/FlowLayout.h"

namespace CityDraft::Assets
{
	class Asset;
}

class ImageSelectionWidget final : public QWidget
{
	Q_OBJECT

public:
	explicit ImageSelectionWidget(QWidget* parent = nullptr);

	void loadImagesFromAssets(const std::vector<std::shared_ptr<CityDraft::Assets::Image>>& invariantImages, const std::vector<std::shared_ptr<
	                          CityDraft::Assets::ImageVariantGroup>>& variantImageGroups
	);

	void resizeEvent(QResizeEvent* event) override;
signals:
		void imageSelected(const QString& imagePath);

private:
	FlowLayout* imagesLayout;
};
