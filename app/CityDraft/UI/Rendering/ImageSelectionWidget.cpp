//
// Created by mariiakorolevaa on 19.04.2025.
//

#include "ImageSelectionWidget.h"
#include "QDir"
#include "QLabel"
#include "QPixmap"
#include "QPushButton"
#include "QScrollArea"
#include "QDebug"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/SkiaImage.h"

namespace CityDraft::Assets {
    class Image;
}

ImageSelectionWidget::ImageSelectionWidget(QWidget* parent) : QWidget(parent) {
    const auto scrollArea = new QScrollArea(this);

    const auto container = new QWidget();
    imagesLayout = new FlowLayout(container, 0, 5, 5);
    imagesLayout->setSpacing(2);
    imagesLayout->setContentsMargins(0, 0, 0, 0);
    imagesLayout->setAlignment(Qt::AlignTop);

    container->setLayout(imagesLayout);
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);

    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
}


void ImageSelectionWidget::loadImagesFromAssets(const std::vector<std::shared_ptr<CityDraft::Assets::Image>>& images)
{
    qDeleteAll(imagesLayout->children());
    int currentRowWidth = 0;
    QHBoxLayout* currentRowLayout = nullptr;

    for (const auto& image : images) {
        if (image == nullptr) {
            qDebug() << "Image is null.";
            continue;
        }

        if (image->GetStatus() != CityDraft::Assets::AssetStatus::Loaded) {
            image->LoadAsset();
        }

        BOOST_ASSERT(image->GetStatus() == CityDraft::Assets::AssetStatus::Loaded);

        const CityDraft::Assets::SkiaImage* skia_image = dynamic_cast<CityDraft::Assets::SkiaImage*>(image.get());
        if (!skia_image) {
            qDebug() << "Failed to cast image to SkiaImage.";
            continue;
        }

        auto q_pix_map = skia_image->GetPixmap();

        if (q_pix_map.isNull()) {
            qDebug() << "Failed to convert QImage to QPixmap.";
            continue;
        }

        const auto imageButton = new QPushButton(this);
        imageButton->setIcon(QIcon(q_pix_map));
        imageButton->setIconSize(QSize(64, 64));
        imageButton->setFixedSize(65, 65);
        imageButton->setToolTip(QString::fromStdString(image->GetUrl().c_str()));
        imageButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        currentRowLayout = new QHBoxLayout();
        currentRowLayout->setSpacing(2);
        currentRowLayout->setContentsMargins(0, 0, 0, 0);
        currentRowLayout->addWidget(imageButton);
        imagesLayout->addLayout(currentRowLayout);

        connect(imageButton, &QPushButton::clicked, this, [this, image]() {
            emit imageSelected(QString::fromStdString(image->GetUrl().c_str()));
        });

        qDebug() << "Added image button.";
    }

    imagesLayout->update();
}

void ImageSelectionWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    (void)imagesLayout->doLayout(rect(), false);
}

