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

	QPointF SkiaWidget::GetCursorProjectedPosition() const
	{
		return m_CursorProjectedPosition;
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
		QPointF vieportPos = event->position();
		m_CursorProjectedPosition = vieportPos / m_ViewportZoom + QPointF(m_ViewportTranslation.GetX(), m_ViewportTranslation.GetY());

		CursorPositionChanged(m_CursorProjectedPosition);
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
		
		for (const auto& draft : m_ViewportDraftsBuffer)
		{
			canvas->save();
			CityDraft::Drafts::SkiaImage* image = dynamic_cast<CityDraft::Drafts::SkiaImage*>(draft.get());
			if (image)
			{
				PaintSkiaImage(canvas, image);
				continue;
			}
			canvas->restore();
		}
	}

	void SkiaWidget::PaintSkiaImage(SkCanvas* canvas, CityDraft::Drafts::SkiaImage* image)
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
		SkMatrix scaleMatrix;
		scaleMatrix.setScale(m_ViewportZoom, m_ViewportZoom);

		canvas->setMatrix(scaleMatrix);

		SkPaint paint;
		Transform2D imageTransform = image->GetTransform();
		Vector2D imageSize = image->GetImageSize() * m_ViewportZoom;
		Vector2D imageTranslationViewportSpace = imageTransform.Translation - m_ViewportTranslation - imageSize/2;
		// SkRect destRect = SkRect::MakeIWH(imageSize.GetX(), imageSize.GetY());
		canvas->drawImage(skImage, imageTranslationViewportSpace.GetX(), imageTranslationViewportSpace.GetY(), SkSamplingOptions(), &paint);
	}

	AxisAlignedBoundingBox2D SkiaWidget::GetViewportBox() const
	{
		Vector2D scaledSize{ size().width() / m_ViewportZoom, size().height() / m_ViewportZoom };
		
		Vector2D screenMin = m_ViewportTranslation;
		Vector2D screenMax = m_ViewportTranslation + scaledSize;
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
