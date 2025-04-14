#pragma once

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <spdlog/spdlog.h>
#include <string_view>
#include <mutex>
#include "OpenGlUtils.h"

namespace CityDraft::UI::Rendering
{
	class SkiaWidget : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		SkiaWidget(QWidget* parent = nullptr);

		sk_sp<GrDirectContext> GetDirectContext() const;
		QOpenGLExtraFunctions& GetGlFunctions();

	signals:
		void GraphicsInitialized(SkiaWidget* source);

	protected:
		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

	private:
		sk_sp<const GrGLInterface> m_GrInterface;
		sk_sp<GrDirectContext> m_GrContext;
		sk_sp<SkSurface> m_SkSurface;
		GrBackendRenderTarget m_BackendRenderTarget;
		QOpenGLExtraFunctions m_GlFuncs;

		std::shared_ptr<spdlog::logger> m_SkiaLogger;
		std::shared_ptr<spdlog::logger> m_GlLogger;

		void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
		static void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}
