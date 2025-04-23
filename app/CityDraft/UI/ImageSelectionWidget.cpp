//
// Created by mariiakorolevaa on 19.04.2025.
//

#include <boost/assert.hpp>
#include <memory>
#include <qalgorithms.h>
#include <qboxlayout.h>
#include <qicon.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "CityDraft/UI/VariantImageButton.h"
#include "CityDraft/UI/VariantImageButton.h"
#include "ImageSelectionWidget.h"

namespace CityDraft::Assets
{
	class ImageVariantGroup;
	class Image;
}

namespace CityDraft::UI
{

	ImageSelectionWidget::ImageSelectionWidget(CityDraft::Assets::AssetManager* assetManager, QWidget* parent) :
		QWidget(parent),
		m_AssetManager(assetManager)
	{
		BOOST_ASSERT(m_AssetManager);

		const auto scrollArea = new QScrollArea(this);

		const auto container = new QWidget(this);
		m_ScrollableLayout = new QGridLayout();
		m_ScrollableLayout->setContentsMargins(QMargins(0, 0, 0, 0));
		m_ScrollableLayout->setSpacing(0);
		container->setLayout(m_ScrollableLayout);
		container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

		scrollArea->setWidget(container);
		scrollArea->setWidgetResizable(true);

		setLayout(new QVBoxLayout());
		layout()->addWidget(scrollArea);

		loadImagesFromAssets();
		m_ResizeTimer = new QTimer(this);
		connect(m_ResizeTimer, &QTimer::timeout, this, &ImageSelectionWidget::OnResizeFinished);
		// UpdateButtonsPlacement();
	}

	ImageSelectionWidget::~ImageSelectionWidget()
	{
		for (auto* button : m_ImageButtons)
		{
			delete button;
		}
		m_ImageButtons.clear();
	}

	void ImageSelectionWidget::loadImagesFromAssets()
	{
		// Variant images
		for (const auto& group : m_AssetManager->GetVariantImages())
		{
			if (!group || group->GetImageVariants().empty()) continue;

			auto* variantButton = new CityDraft::UI::VariantImageButton(group, this);

			connect(variantButton, &CityDraft::UI::VariantImageButton::imageVariantSelected,
				[this, variantButton](const std::shared_ptr<CityDraft::Assets::Image>& image) {
					if (image)
					{
						emit ImageSelected(image);
						variantButton->updatePreviewIcon(image);
					}
				});

			connect(variantButton, &CityDraft::UI::VariantImageButton::imageGroupSelected, this, [this, group]() {
				emit ImageGroupSelected(group);
				});
			
			m_ImageButtons.push_back(variantButton);
		}
		
		// Invariant images
		for (const auto& image : m_AssetManager->GetInvariantImages())
		{
			auto* skiaImage = dynamic_cast<CityDraft::Assets::SkiaImage*>(image.get());
			BOOST_ASSERT(skiaImage);

			if (skiaImage->GetStatus() != CityDraft::Assets::AssetStatus::Loaded)
			{
				skiaImage->LoadAsset();
			}

			auto pixmap = skiaImage->GetPixmap();
			BOOST_ASSERT(!pixmap.isNull());

			auto* button = new QPushButton(this);
			button->setIcon(QIcon(pixmap));
			button->setIconSize(QSize(ImageSize, ImageSize));
			button->setFixedSize(ImageSize + 1, ImageSize + 1);
			button->setToolTip(QString::fromStdString(image->GetUrl().data()));
			connect(button, &QPushButton::clicked, this, [this, image]() {
				emit ImageSelected(image);
				});

			m_ImageButtons.push_back(button);
		}
	}

	void ImageSelectionWidget::resizeEvent(QResizeEvent* event)
	{
		QWidget::resizeEvent(event);
		
		m_ResizeTimer->start(200);
	}

	void ImageSelectionWidget::UpdateButtonsPlacement()
	{
		for (auto* button : m_ImageButtons)
		{
			m_ScrollableLayout->removeWidget(button);
			button->setParent(nullptr);
		}
		
		constexpr int buttonSize = ImageSize;
		int spacePerButton = buttonSize;
		int rowSize = size().width() / buttonSize;
		
		int col = 0;
		int row = 0;
		for (auto* button : m_ImageButtons)
		{
			m_ScrollableLayout->addWidget(button, row, col);
			col++;
			if (col >= rowSize - 1)
			{
				col = 0;
				row++;
			}
		}
	}

	void ImageSelectionWidget::OnResizeFinished()
	{
		UpdateButtonsPlacement();
		m_ResizeTimer->stop();
	}

}