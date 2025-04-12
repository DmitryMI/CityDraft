#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QPainter>

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImageInfo.h"

namespace CityDraft::UI::Rendering
{
	class RenderingWidget : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		RenderingWidget(QWidget* parent = nullptr);

		static RenderingWidget* Factory(const QString& rendererName, QWidget* parent);
	};
}
