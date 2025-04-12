#include "SkiaWidget.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/core/SkColorSpace.h"


namespace CityDraft::UI::Rendering
{
	SkiaWidget::SkiaWidget(QWidget* parent) : RenderingWidget(parent)
	{

	}

	void SkiaWidget::initializeGL()
	{
		m_GlFuncs.initializeOpenGLFunctions();

		m_GrInterface = GrGLMakeNativeInterface();
		m_GrContext = GrDirectContexts::MakeGL(m_GrInterface);

		
	}


	void SkiaWidget::resizeGL(int w, int h)
	{
		if (!m_GrContext)
		{
			return;
		}

		GrGLFramebufferInfo fbInfo;
		fbInfo.fFBOID = defaultFramebufferObject();
		fbInfo.fFormat = GL_RGBA8;

		m_BackendRenderTarget = GrBackendRenderTargets::MakeGL(w, h, 0, 0, fbInfo);

		SkSurfaceProps props;
		m_SkSurface = SkSurfaces::WrapBackendRenderTarget(
			m_GrContext.get(), m_BackendRenderTarget, kBottomLeft_GrSurfaceOrigin,
			kRGBA_8888_SkColorType, nullptr, &props);

		if (!m_SkSurface)
		{
			qCritical() << "Failed to create m_SkSurface";
		}
	}

	void SkiaWidget::paintGL()
	{
		m_GlFuncs.glEnable(GL_DEBUG_OUTPUT);

		m_GlFuncs.glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			qDebug() << "GL Error:" << message;
			}, nullptr);

		if (!m_SkSurface)
		{
			return;
		}

		SkCanvas* canvas = m_SkSurface->getCanvas();
		canvas->clear(SK_ColorGREEN);

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(SK_ColorRED);
		paint.setStyle(SkPaint::Style::kFill_Style);
		canvas->drawCircle(400, 250, 100, paint);

		m_GrContext->resetContext(kAll_GrBackendState);
		m_GrContext->flushAndSubmit(m_SkSurface.get());
	}

	
}
