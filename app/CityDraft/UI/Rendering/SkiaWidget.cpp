#include "SkiaWidget.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/core/SkColorSpace.h"
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/Assets/SkiaImage.h"


namespace CityDraft::UI::Rendering
{
	SkiaWidget::SkiaWidget(QWidget* parent) : QOpenGLWidget(parent)
	{
		m_WidgetLogger = Logging::LogManager::CreateLogger("SkiaWidget");
		m_SkiaLogger = Logging::LogManager::CreateLogger("Skia");
		m_GlLogger = Logging::LogManager::CreateLogger("GL");
		setMouseTracking(true);
	}

	sk_sp<GrDirectContext> SkiaWidget::GetDirectContext() const
	{
		return m_GrContext;
	}

	QOpenGLExtraFunctions& SkiaWidget::GetGlFunctions()
	{
		return m_GlFuncs;
	}

	std::shared_ptr<CityDraft::Scene> SkiaWidget::GetScene() const
	{
		return m_Scene;
	}

	void SkiaWidget::SetScene(std::shared_ptr<CityDraft::Scene> scene)
	{
		m_Scene = scene;
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

		emit GraphicsInitialized(this);
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
		canvas->clear(SK_ColorTRANSPARENT);

		PaintScene();

		m_GrContext->flushAndSubmit(m_SkSurface.get());
	}

	void SkiaWidget::mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
		{
			m_WidgetLogger->debug("LMB Pressed at ({}, {})", event->position().x(), event->position().y());
		}
		else if (event->button() == Qt::RightButton)
		{
			m_WidgetLogger->debug("RMB Pressed at ({}, {})", event->position().x(), event->position().y());
		}
		else if (event->button() == Qt::MiddleButton)
		{
			m_WidgetLogger->debug("MMB Pressed at ({}, {})", event->position().x(), event->position().y());
		}
		else
		{
			m_WidgetLogger->debug("Unknown Mouse Button Pressed at ({}, {})", event->position().x(), event->position().y());
		}
	}

	void SkiaWidget::mouseReleaseEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
		{
			m_WidgetLogger->debug("LMB Released at ({}, {})", event->position().x(), event->position().y());
		}
		else if (event->button() == Qt::RightButton)
		{
			m_WidgetLogger->debug("RMB Released at ({}, {})", event->position().x(), event->position().y());
		}
		else if (event->button() == Qt::MiddleButton)
		{
			m_WidgetLogger->debug("MMB Released at ({}, {})", event->position().x(), event->position().y());
		}
		else
		{
			m_WidgetLogger->debug("Unknown Mouse Button Released at ({}, {})", event->position().x(), event->position().y());
		}
	}

	void SkiaWidget::mouseMoveEvent(QMouseEvent* event)
	{
		m_WidgetLogger->debug("Mouse moved to ({}, {})", event->position().x(), event->position().y());
	}

	void SkiaWidget::PaintScene()
	{
		if (!m_Scene)
		{
			return;
		}

		m_ViewportDraftsBuffer.clear();

		auto vieportBox = GetViewportBox();

		size_t draftsInViewportNum = m_Scene->QueryDraftsOnAllLayers(vieportBox, m_ViewportDraftsBuffer);
		m_WidgetLogger->debug("{} drafts on the viewport", draftsInViewportNum);

		for (const auto& draft : m_ViewportDraftsBuffer)
		{
			CityDraft::Drafts::SkiaImage* image = dynamic_cast<CityDraft::Drafts::SkiaImage*>(draft.get());
			if (image)
			{
				PaintSkiaImage(image);
				continue;
			}
		}
	}

	void SkiaWidget::PaintSkiaImage(CityDraft::Drafts::SkiaImage* image)
	{
		CityDraft::Assets::SkiaImage* asset = dynamic_cast<CityDraft::Assets::SkiaImage*>(image->GetAsset());
		BOOST_ASSERT(asset);
		// TODO don't crash on assets with failed resources
		if (asset->GetStatus() == Assets::AssetStatus::Initialized)
		{
			asset->LoadAsset();
		}
		BOOST_ASSERT(asset->GetStatus() == Assets::AssetStatus::Loaded);
		BOOST_ASSERT(m_SkSurface);

		auto skImage = asset->GetGpuImage();

		SkCanvas* canvas = m_SkSurface->getCanvas();
		BOOST_ASSERT(canvas);
		SkPaint paint;
		Transform2D imageTransform = image->GetTransform();
		Vector2D imageTranslationViewportSpace = imageTransform.Translation - m_ViewportTranslation;
		canvas->drawImage(skImage, imageTranslationViewportSpace.GetX(), imageTranslationViewportSpace.GetY(), SkSamplingOptions(), &paint);
	}

	AxisAlignedBoundingBox2D SkiaWidget::GetViewportBox() const
	{
		Vector2D scaledHalfSize{ size().width() / 2.0 / m_ViewportZoom, size().height() / 2.0 / m_ViewportZoom };
		
		Vector2D screenMin = m_ViewportTranslation - scaledHalfSize;
		Vector2D screenMax = m_ViewportTranslation + scaledHalfSize;
		return AxisAlignedBoundingBox2D(screenMin, screenMax);
	}

	void SkiaWidget::GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const
	{
		auto sourceStr = OpenGlUtils::GlMessageSourceToString(source);
		auto typeStr = OpenGlUtils::GlMessageTypeToString(type);
		auto level = OpenGlUtils::GlMessageSeverityToLogLevel(severity);
		std::string msgStr(message, length);
		m_GlLogger->log(level, "[Source: {}] [Type: {}] [ID: {}] {}", sourceStr, typeStr, id, msgStr);
	}

	void SkiaWidget::GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		const SkiaWidget* self = reinterpret_cast<const SkiaWidget*>(userParam);
		self->GlLogCallback(source, type, id, severity, length, message);
	}

	
}
