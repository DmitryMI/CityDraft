#include "SkiaWidget.h"
#include "include/core/SkCanvas.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/core/SkColorSpace.h"
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "OpenGlUtils.h"

namespace CityDraft::UI::Rendering
{
	SkiaWidget::SkiaWidget(QWidget* parent) :
		QOpenGLWidget(parent)
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

	Vector2D SkiaWidget::Project(const QPointF& pixelCoord) const
	{
		QPointF widgetSize = QPointF(size().width() / 2.0, size().height() / 2.0);
		QPointF viewportPos = pixelCoord - widgetSize;
		QPointF projected = viewportPos / m_ViewportZoom + QPointF(m_ViewportCenter.GetX(), m_ViewportCenter.GetY());
		return Vector2D(projected.x(), projected.y());
	}

	void SkiaWidget::Paint(CityDraft::Assets::Asset* asset, const Transform2D& transform)
	{
		m_Canvas->save();
		m_Canvas->translate(transform.Translation.GetX(), transform.Translation.GetY());
		m_Canvas->scale(transform.Scale.GetX(), transform.Scale.GetY());

		BOOST_ASSERT(asset);
		// TODO don't crash on assets with failed resources
		if (asset->GetStatus() == Assets::AssetStatus::Initialized)
		{
			asset->LoadAsset();
		}
		BOOST_ASSERT(asset->GetStatus() == Assets::AssetStatus::Loaded);

		if (CityDraft::Assets::SkiaImage* imageAsset = dynamic_cast<CityDraft::Assets::SkiaImage*>(asset))
		{
			Paint(imageAsset, transform);
		}

		m_Canvas->restore();
	}

	const Vector2D SkiaWidget::GetViewportCenter() const
	{
		return m_ViewportCenter;
	}

	double SkiaWidget::GetViewportZoom() const
	{
		return m_ViewportZoom;
	}

	void SkiaWidget::SetViewportTransform(const Vector2D& center, double zoom)
	{
		m_ViewportCenter = center;
		m_ViewportZoom = zoom;
	}

	void SkiaWidget::Repaint()
	{
		update();
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

		m_Canvas = m_SkSurface->getCanvas();
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

		m_Canvas->clear(SK_ColorTRANSPARENT);

		PaintScene();

		m_GrContext->flushAndSubmit(m_SkSurface.get());
	}

	void SkiaWidget::mousePressEvent(QMouseEvent* event)
	{
		emit MouseButtonEvent(event, true);
	}

	void SkiaWidget::mouseReleaseEvent(QMouseEvent* event)
	{
		emit MouseButtonEvent(event, false);
	}

	void SkiaWidget::mouseMoveEvent(QMouseEvent* event)
	{
		emit MouseMoveEvent(event);
	}

	void SkiaWidget::wheelEvent(QWheelEvent* event)
	{
		int delta = event->angleDelta().y();
		m_ViewportZoom += 0.0001 * delta;
		if (m_ViewportZoom == 0)
		{
			m_ViewportZoom = 0.0001;
		}
		m_WidgetLogger->debug("Zoom changed to {}", m_ViewportZoom);
		update();
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

		m_WidgetLogger->debug("{} drafts inside the viewport box: [({},{}), ({},{})]", draftsInViewportNum,
			vieportBox.GetMin().GetX(),
			vieportBox.GetMin().GetY(),
			vieportBox.GetMax().GetX(),
			vieportBox.GetMax().GetY()
			);

		SkCanvas* canvas = m_SkSurface->getCanvas();
		BOOST_ASSERT(canvas);

		canvas->save();
		canvas->translate(size().width() / 2, size().height() / 2);
		canvas->scale(m_ViewportZoom, m_ViewportZoom);
		canvas->translate(-m_ViewportCenter.GetX(), -m_ViewportCenter.GetY());

		for (const auto& draft : m_ViewportDraftsBuffer)
		{
			Paint(draft->GetAsset(), draft->GetTransform());
		}

		canvas->restore();
		canvas->resetMatrix();
	}


	void SkiaWidget::Paint(CityDraft::Assets::SkiaImage* image, const Transform2D& transform)
	{
		BOOST_ASSERT(image);
		// TODO don't crash on assets with failed resources
		if (image->GetStatus() == Assets::AssetStatus::Initialized)
		{
			image->LoadAsset();
		}
		BOOST_ASSERT(image->GetStatus() == Assets::AssetStatus::Loaded);

		auto skImage = image->GetGpuImage();

		Vector2D imageSize = image->GetImageSize();

		SkRect destRect = SkRect::MakeXYWH(
			-imageSize.GetX() / 2.0f,
			-imageSize.GetY() / 2.0f,
			imageSize.GetX(),
			imageSize.GetY()
		);
		m_Canvas->drawImageRect(skImage, destRect, SkSamplingOptions());
	}

	Vector2D SkiaWidget::GetViewportProjectedSize() const
	{
		return { size().width() / m_ViewportZoom, size().height() / m_ViewportZoom };
	}

	AxisAlignedBoundingBox2D SkiaWidget::GetViewportBox() const
	{
		Vector2D scaledSize = GetViewportProjectedSize();
		
		Vector2D screenMin = m_ViewportCenter - scaledSize / 2;
		Vector2D screenMax = m_ViewportCenter + scaledSize / 2;
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
