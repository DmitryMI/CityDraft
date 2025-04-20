#include "VariantImageButton.h"
#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/Assets/SkiaImage.h"

#include <QDebug>
#include <QIcon>
#include <QPixmap>

namespace CityDraft::UI {

    VariantImageButton::VariantImageButton(std::shared_ptr<CityDraft::Assets::ImageVariantGroup> imageVariantGroup, QWidget* parent)
        : QPushButton(parent),
          m_ImageVariantGroup(std::move(imageVariantGroup)) {

        if (!m_ImageVariantGroup || m_ImageVariantGroup->GetImageVariants().empty()) {
            qWarning() << "VariantImageButton: ImageVariantGroup is empty or null.";
            return;
        }

        auto defaultImage = m_ImageVariantGroup->GetDefaultImage();
        if (!defaultImage) {
            qWarning() << "VariantImageButton: Default image is null.";
            return;
        }

        if (defaultImage->GetStatus() != CityDraft::Assets::AssetStatus::Loaded) {
            defaultImage->LoadAsset();
        }

        auto skiaImage = std::dynamic_pointer_cast<CityDraft::Assets::SkiaImage>(defaultImage);
        if (!skiaImage) {
            qWarning() << "VariantImageButton: Failed to cast to SkiaImage.";
            return;
        }

        QPixmap pixmap = skiaImage->GetPixmap();
        if (pixmap.isNull()) {
            qWarning() << "VariantImageButton: Pixmap is null.";
            return;
        }

        this->setIcon(QIcon(pixmap));
        this->setIconSize(QSize(64, 64));
        this->setFixedSize(65, 65);
        this->setToolTip("Image Variant Group");
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(this, &QPushButton::clicked, this, [this]() {
            emit imageGroupSelected(m_ImageVariantGroup);
        });
    }

}