#include "SkiaWidget.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/core/SkColorSpace.h"
#include "CityDraft/Logging/LogManager.h"


namespace CityDraft::UI::Rendering
{
	SkiaWidget::SkiaWidget(QWidget* parent) : RenderingWidget(parent)
	{
		m_SkiaLogger = Logging::LogManager::CreateLogger("Skia");
		m_GlLogger = Logging::LogManager::CreateLogger("GL");
	}

	void SkiaWidget::initializeGL()
	{
		if (!isValid())
		{
			return;
		}

		m_GlFuncs.initializeOpenGLFunctions();

		m_GrInterface = GrGLMakeNativeInterface();
		m_GrContext = GrDirectContexts::MakeGL(m_GrInterface);

		m_GlFuncs.glEnable(GL_DEBUG_OUTPUT);

		m_GlFuncs.glDebugMessageCallback(
			&SkiaWidget::GlLogCallback,
			this
		);

	}


	void SkiaWidget::resizeGL(int w, int h)
	{
		if (!isValid())
		{
			return;
		}

		if (!m_GrContext)
		{
			return;
		}

		auto surfaceFormat = QOpenGLContext::currentContext()->format();

		GrGLFramebufferInfo fbInfo;
		fbInfo.fFBOID = defaultFramebufferObject();
		fbInfo.fFormat = GL_RGBA8;

		m_BackendRenderTarget = GrBackendRenderTargets::MakeGL(w, h, surfaceFormat.samples(), surfaceFormat.stencilBufferSize(), fbInfo);

		sk_sp<SkColorSpace> srgb = SkColorSpace::MakeSRGB();
		m_SkSurface = SkSurfaces::WrapBackendRenderTarget(
			m_GrContext.get(),
			m_BackendRenderTarget,
			kBottomLeft_GrSurfaceOrigin,
			kRGBA_8888_SkColorType,
			srgb,
			nullptr
		);

		if (!m_SkSurface)
		{
			m_SkiaLogger->critical("Failed to create m_SkSurface");
		}
	}

	void SkiaWidget::paintGL()
	{
		if (!isValid())
		{
			return;
		}

		if (!m_SkSurface)
		{
			return;
		}

		m_GrContext->resetContext(kAll_GrBackendState);

		SkCanvas* canvas = m_SkSurface->getCanvas();
		canvas->clear(SK_ColorGREEN);

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(SK_ColorRED);
		paint.setStyle(SkPaint::Style::kFill_Style);
		canvas->drawCircle(400, 250, 100, paint);

		m_GrContext->flushAndSubmit(m_SkSurface.get());
	}

	void SkiaWidget::GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const
	{
		auto sourceStr = GlMessageSourceToString(source);
		auto typeStr = GlMessageTypeToString(type);
		auto level = GlMessageSeverityToLogLevel(severity);
		std::string msgStr(message, length);
		m_GlLogger->log(level, "[Source: {}] [Type: {}] [ID: {}] {}", sourceStr, typeStr, id, msgStr);
	}

	std::string_view SkiaWidget::GlMessageSourceToString(GLenum source)
	{
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			return "API";
			break;

		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "WINDOW SYSTEM";
			break;

		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "SHADER COMPILER";
			break;

		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "THIRD PARTY";
			break;

		case GL_DEBUG_SOURCE_APPLICATION:
			return "APPLICATION";
			break;

		case GL_DEBUG_SOURCE_OTHER:
			return "OTHER";
			break;

		default:
			return "UNKNOWN";
			break;
		}
	}

	std::string_view SkiaWidget::GlMessageTypeToString(GLenum type)
	{
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			return "ERROR";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "DEPRECATED BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "UDEFINED BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_PORTABILITY:
			return "PORTABILITY";
			break;

		case GL_DEBUG_TYPE_PERFORMANCE:
			return "PERFORMANCE";
			break;

		case GL_DEBUG_TYPE_OTHER:
			return "OTHER";
			break;

		case GL_DEBUG_TYPE_MARKER:
			return "MARKER";
			break;

		default:
			return "UNKNOWN";
			break;
		}
	}

	spdlog::level::level_enum SkiaWidget::GlMessageSeverityToLogLevel(GLenum severity)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			return spdlog::level::err;
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			return spdlog::level::warn;
			break;

		case GL_DEBUG_SEVERITY_LOW:
			return spdlog::level::info;
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return spdlog::level::debug;
			break;

		default:
			return spdlog::level::debug;
			break;
		}
	}

	void SkiaWidget::GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		const SkiaWidget* self = reinterpret_cast<const SkiaWidget*>(userParam);
		self->GlLogCallback(source, type, id, severity, length, message);
	}

	
}
