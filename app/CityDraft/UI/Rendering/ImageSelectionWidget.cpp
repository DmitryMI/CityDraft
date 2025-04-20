//
// Created by mariiakorolevaa on 19.04.2025.
//

#include "ImageSelectionWidget.h"
#include "QDir"
#include "QLabel"
#include "QPixmap"
#include "QScrollArea"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "CityDraft/UI/VariantImageButton.h"

namespace CityDraft::Assets {
    class ImageVariantGroup;
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


void ImageSelectionWidget::loadImagesFromAssets(
    const std::vector<std::shared_ptr<CityDraft::Assets::Image>>& invariantImages,
    const std::vector<std::shared_ptr<CityDraft::Assets::ImageVariantGroup>>& variantImageGroups)
{
    qDeleteAll(imagesLayout->children());

    // Invariant images
    for (const auto& image : invariantImages) {
        auto* skiaImage = dynamic_cast<CityDraft::Assets::SkiaImage*>(image.get());
        if (!skiaImage) continue;

        if (skiaImage->GetStatus() != CityDraft::Assets::AssetStatus::Loaded)
        {
            skiaImage->LoadAsset();
        }

        auto pixmap = skiaImage->GetPixmap();
        if (pixmap.isNull()) continue;

        auto* button = new QPushButton(this);
        button->setIcon(QIcon(pixmap));
        button->setIconSize(QSize(64, 64));
        button->setFixedSize(65, 65);
        button->setToolTip(QString::fromStdString(image->GetUrl().data()));
        connect(button, &QPushButton::clicked, this, [this, image]() {
            emit imageSelected(QString::fromStdString(image->GetUrl().data()));
        });

        auto* row = new QHBoxLayout();
        row->addWidget(button);
        imagesLayout->addLayout(row);
    }

    // Variant images
    for (const auto& group : variantImageGroups) {
        if (!group || group->GetImageVariants().empty()) continue;

        auto* variantButton = new CityDraft::UI::VariantImageButton(group, this);

        connect(variantButton, &CityDraft::UI::VariantImageButton::imageVariantSelected, this,
            [this](const std::shared_ptr<CityDraft::Assets::Image>& image) {
                if (image) {
                    emit imageSelected(QString::fromStdString(image->GetUrl().data()));
                }
            });

        connect(variantButton, &CityDraft::UI::VariantImageButton::imageVariantSelected, variantButton,
            [variantButton](const std::shared_ptr<CityDraft::Assets::Image>& image) {
                if (image) {
                    variantButton->updatePreviewIcon(image);
                }
            });

        auto* row = new QHBoxLayout();
        row->addWidget(variantButton);
        imagesLayout->addLayout(row);
    }

    imagesLayout->update();
}

void ImageSelectionWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    (void)imagesLayout->doLayout(rect(), false);
}

