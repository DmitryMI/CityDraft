#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QSignalMapper>

#include "CityDraft/Assets/Image.h"

namespace CityDraft::Assets
{
	class Asset;
}

class ImageSelectionWidget final : public QWidget
{
	Q_OBJECT

public:
	explicit ImageSelectionWidget(QWidget* parent = nullptr);
	void loadImagesFromAssets(const std::vector<std::shared_ptr<CityDraft::Assets::Image>>& images);

	signals:
		void imageSelected(const QString& imagePath);

private:
	QVBoxLayout* imagesLayout;
};
