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
#include "CityDraft/UI/FlowLayout.h"
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

		const auto container = new QWidget();
		m_ImagesLayout = new FlowLayout(container, 0, 5, 5);
		m_ImagesLayout->setSpacing(2);
		m_ImagesLayout->setContentsMargins(0, 0, 0, 0);
		m_ImagesLayout->setAlignment(Qt::AlignTop);

		container->setLayout(m_ImagesLayout);
		container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

		scrollArea->setWidget(container);
		scrollArea->setWidgetResizable(true);

		const auto mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(scrollArea);
		mainLayout->setContentsMargins(0, 0, 0, 0);
		setLayout(mainLayout);

		loadImagesFromAssets();
	}

	void ImageSelectionWidget::loadImagesFromAssets()
	{
		qDeleteAll(m_ImagesLayout->children());

		
		// Variant images
		for (const auto& group : m_AssetManager->GetVariantImages())
		{
			if (!group || group->GetImageVariants().empty()) continue;

			auto* variantButton = new CityDraft::UI::VariantImageButton(group, this);

			connect(variantButton, &CityDraft::UI::VariantImageButton::imageVariantSelected,
				[this, variantButton](const std::shared_ptr<CityDraft::Assets::Image>& image) {
					if (image)
					{
						emit imageSelected(QString::fromStdString(image->GetUrl().data()));
						variantButton->updatePreviewIcon(image);
					}
				});

			auto* row = new QHBoxLayout();
			row->addWidget(variantButton);
			connect(variantButton, &CityDraft::UI::VariantImageButton::imageGroupSelected, this, [this, group]() {
				emit imageGroupSelected(group);
				});
			m_ImagesLayout->addLayout(row);
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
			button->setIconSize(QSize(64, 64));
			button->setFixedSize(65, 65);
			button->setToolTip(QString::fromStdString(image->GetUrl().data()));
			connect(button, &QPushButton::clicked, this, [this, image]() {
				emit imageSelected(QString::fromStdString(image->GetUrl().data()));
				});

			auto* row = new QHBoxLayout(this);
			row->addWidget(button);
			m_ImagesLayout->addLayout(row);
		}

		m_ImagesLayout->update();
		m_ImagesLayout->parentWidget()->update();
	}

	void ImageSelectionWidget::resizeEvent(QResizeEvent* event)
	{
		QWidget::resizeEvent(event);
		(void)m_ImagesLayout->doLayout(rect(), false);
	}

}