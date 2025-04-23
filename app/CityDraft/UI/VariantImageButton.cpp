#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "VariantImageButton.h"

#include <QAction>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QPixmap>
#include <memory>

namespace CityDraft::UI
{
    VariantImageButton::VariantImageButton(std::shared_ptr<CityDraft::Assets::ImageVariantGroup> imageVariantGroup, QWidget* parent)
        : QPushButton(parent),
          m_ImageVariantGroup(std::move(imageVariantGroup))
    {
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
        plusButton = new QPushButton("+", this);
        plusButton->setFixedSize(16, 16);
        plusButton->move(width() - 18, 2);
        plusButton->raise();
        plusButton->setStyleSheet(R"(
        QPushButton {
        background-color: white;
        border: 1px solid gray;
        border-radius: 8px;
        font-size: 12px;
        font-weight: bold;
        color: black;
    }
)");
        connect(plusButton, &QPushButton::clicked, this, &VariantImageButton::showVariantMenu);
    }

    void VariantImageButton::showVariantMenu() {
        QMenu* menu = new QMenu(this);

        for (const auto& image : m_ImageVariantGroup->GetImageVariants()) {
            if (!image) continue;

            if (image->GetStatus() != CityDraft::Assets::AssetStatus::Loaded) {
                image->LoadAsset();
            }

            auto skiaImage = std::dynamic_pointer_cast<CityDraft::Assets::SkiaImage>(image);
            if (!skiaImage) continue;

            QPixmap pixmap = skiaImage->GetPixmap();
            if (pixmap.isNull()) continue;

            QAction* action = new QAction(QIcon(pixmap), QString::fromStdString(image->GetUrl().data()), menu);
            connect(action, &QAction::triggered, this, [this, image]() {
                emit imageVariantSelected(image);
            });

            menu->addAction(action);
        }

        menu->exec(mapToGlobal(QPoint(0, height())));
    }

    void VariantImageButton::updatePreviewIcon(std::shared_ptr<Assets::Image> image)
    {
        if (!image) return;

        const auto skiaImage = std::dynamic_pointer_cast<Assets::SkiaImage>(image);
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
    }

    void VariantImageButton::resizeEvent(QResizeEvent* event)
    {
        QPushButton::resizeEvent(event);
        if (plusButton) {
            plusButton->move(width() - 18, 2);
        }
    }
} // namespace CityDraft::UI