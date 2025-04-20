#pragma once

#include <QPushButton>
#include <memory>

namespace CityDraft::Assets {
    class ImageVariantGroup;
}

namespace CityDraft::UI {

    class VariantImageButton final : public QPushButton {
        Q_OBJECT

    public:
        explicit VariantImageButton(std::shared_ptr<Assets::ImageVariantGroup> imageVariantGroup, QWidget* parent = nullptr);

        signals:
            void imageGroupSelected(std::shared_ptr<Assets::ImageVariantGroup> group);

    private:
        std::shared_ptr<Assets::ImageVariantGroup> m_ImageVariantGroup;
    };

}