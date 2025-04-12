#include "RenderingWidget.h"
#include "SkiaWidget.h"

namespace CityDraft::UI::Rendering
{

	RenderingWidget::RenderingWidget(QWidget* parent) : QOpenGLWidget(parent)
	{
	}

	RenderingWidget* RenderingWidget::Factory(const QString& rendererName, QWidget* parent)
	{
		if (rendererName == "skia")
		{
			return new SkiaWidget(parent);
		}

		return nullptr;
	}

}
