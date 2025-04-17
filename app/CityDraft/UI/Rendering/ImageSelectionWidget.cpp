#include "ImageSelectionWidget.h"
#include "QDir"
#include "QLabel"
#include "QPixmap"
#include "QPushButton"
#include "QScrollArea"
#include "QDebug"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/FileSystemError.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/SkiaImage.h"

namespace CityDraft::Assets
{
	class Image;
}

ImageSelectionWidget::ImageSelectionWidget(QWidget* parent) : QWidget(parent) {
	const auto scrollArea = new QScrollArea(this);

	const auto container = new QWidget();
	imagesLayout = new QVBoxLayout(container);
	container->setLayout(imagesLayout);

	scrollArea->setWidget(container);
	scrollArea->setWidgetResizable(true);

	const auto mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(scrollArea);
	setLayout(mainLayout);
}

void ImageSelectionWidget::loadImagesFromAssets(const std::vector<std::shared_ptr<CityDraft::Assets::Image>>& images)
{
	qDeleteAll(imagesLayout->children());

	for (const auto& image : images) {
		if (image->GetStatus() != CityDraft::Assets::AssetStatus::Loaded)
		{
			image->LoadAsset();
		}
		BOOST_ASSERT(image->GetStatus() == CityDraft::Assets::AssetStatus::Loaded);
		const CityDraft::Assets::SkiaImage* skia_image = dynamic_cast<CityDraft::Assets::SkiaImage*>(image.get());
		auto q_pix_map = skia_image->GetPixmap();


		if (q_pix_map.isNull()) {
			qDebug() << "Failed to convert QImage to QPixmap.";
			continue;
		}

		const auto imageButton = new QPushButton(this);
		imageButton->setIcon(QIcon(q_pix_map));
		imageButton->setIconSize(QSize(64, 64));
		imageButton->setToolTip(QString::fromStdString(image->GetUrl().c_str()));
		imageButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		imageButton->setText("ButtonText");

		connect(imageButton, &QPushButton::clicked, this, [this, image]() {
			emit imageSelected(QString::fromStdString(image->GetUrl().c_str()));
		});

		imagesLayout->addWidget(imageButton);
		qDebug() << "Added image button.";
	}

	imagesLayout->update();
}


