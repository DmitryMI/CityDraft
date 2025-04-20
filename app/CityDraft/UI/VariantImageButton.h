#pragma once

#include <QPushButton>
#include <memory>

#include "CityDraft/Assets/Image.h"

namespace CityDraft::Assets {
    class ImageVariantGroup;
}

namespace CityDraft::UI {

    class VariantImageButton final : public QPushButton {
        Q_OBJECT

    public:
        explicit VariantImageButton(std::shared_ptr<Assets::ImageVariantGroup> imageVariantGroup, QWidget* parent = nullptr);
        void showVariantMenu();

    signals:
        void imageGroupSelected(std::shared_ptr<Assets::ImageVariantGroup> group);
        void imageVariantSelected(std::shared_ptr<Assets::Image> image);

    public slots:
    void updatePreviewIcon(std::shared_ptr<Assets::Image> image);
        void resizeEvent(QResizeEvent* event) override;

    private:
        std::shared_ptr<Assets::ImageVariantGroup> m_ImageVariantGroup;
        QPushButton* plusButton = nullptr;
    };

}