#include <boost/assert.hpp>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkColorSpace.h>
#include <include/core/SkColorType.h>
#include <include/core/SkRefCnt.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/gpu/ganesh/GrTypes.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/gpu/ganesh/gl/GrGLTypes.h>
#include <memory>
#include <qevent.h>
#include <qopenglcontext.h>
#include <qopenglext.h>
#include <qopenglextrafunctions.h>
#include <qopenglwidget.h>
#include <qpoint.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <set>
#include <string>
#include <utility>
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Drafts/Draft.h"
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/Scene.h"
#include "CityDraft/Transform2D.h"
#include "CityDraft/Vector2D.h"
#include "OpenGlUtils.h"
#include "SkiaPainters/Circle.h"
#include "SkiaPainters/Image.h"
#include "SkiaPainters/Painter.h"
#include "SkiaPainters/Rect.h"
#include "SkiaPainters/ColorCurve.h"
#include "SkiaWidget.h"
#include "CityDraft/Assets/SkiaColorCurve.h"
#include "CityDraft/Drafts/SkiaImage.h"
#include "CityDraft/DraftZSortKey.h"

// Must be included the latest, or else does not compile
#include <GL/gl.h>

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

	QPointF SkiaWidget::Deproject(const Vector2D& pixelCoord) const
	{
		QPointF widgetSize = QPointF(size().width() / 2.0, size().height() / 2.0);

		QPointF projected = QPointF(pixelCoord.GetX(), pixelCoord.GetY());
		projected -= QPointF(m_ViewportCenter.GetX(), m_ViewportCenter.GetY());
		projected *= m_ViewportZoom;
		projected += widgetSize;
		return projected;
	}

	void SkiaWidget::Paint(CityDraft::Assets::Asset* asset, const Transform2D& transform)
	{
		if (CityDraft::Assets::SkiaImage* imageAsset = dynamic_cast<CityDraft::Assets::SkiaImage*>(asset))
		{
			Paint(imageAsset, transform);
		}
		else
		{
			BOOST_ASSERT(false);
		}
	}

	void SkiaWidget::PaintRectViewportSpace(const QPointF& pixelMin, const QPointF& pixelMax, const LinearColorF& color, double thickness)
	{
		Vector2D min = Project(pixelMin);
		Vector2D max = Project(pixelMax);
		PaintRect(min, max, color, thickness / GetViewportZoom());
	}

	void SkiaWidget::PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& outlineColor, double outlineThickness)
	{
		auto painter = std::make_shared<SkiaPainters::Rect>(min, max, outlineColor, outlineThickness);
		PaintOrQueue(painter);
	}

	void SkiaWidget::PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& outlineColor, double outlineThickness, const LinearColorF& fillColor)
	{
		auto painter = std::make_shared<SkiaPainters::Rect>(min, max, outlineColor, outlineThickness, fillColor);
		PaintOrQueue(painter);
	}

	void SkiaWidget::PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& fillColor)
	{
		auto painter = std::make_shared<SkiaPainters::Rect>(min, max, fillColor);
		PaintOrQueue(painter);
	}

	void SkiaWidget::PaintCircle(const Vector2D& pos, double radius, const LinearColorF& color, double thickness)
	{
		auto painter = std::make_shared<SkiaPainters::Circle>(pos, radius, color, thickness);
		PaintOrQueue(painter);
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

		m_BackendRenderTarget = CreateRenderTarget(w, h);
		m_SkSurface = CreateSurface(m_BackendRenderTarget);
		BOOST_ASSERT(m_SkSurface);
		m_Canvas = m_SkSurface->getCanvas();
		BOOST_ASSERT(m_Canvas);

		m_CurveMaskRenderTarget = CreateRenderTarget(w, h);
		m_CurveMaskSurface = CreateSurface(m_CurveMaskRenderTarget);
		BOOST_ASSERT(m_CurveMaskSurface);
		m_CurveMaskCanvas = m_CurveMaskSurface->getCanvas();
		BOOST_ASSERT(m_CurveMaskCanvas);
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

		m_IsGlPainting = true;

		m_GrContext->resetContext(kAll_GrBackendState);

		CanvasStart(m_Canvas, SK_ColorTRANSPARENT);
		CanvasStart(m_CurveMaskCanvas, SK_ColorTRANSPARENT);

		PaintScene();

		while (!m_QueuedPainters.empty())
		{
			auto painter = m_QueuedPainters.front();
			m_QueuedPainters.pop();
			painter->Paint(this, m_Canvas);
		}

		emit GraphicsPainting(this);

		m_IsGlPainting = false;
		m_GrContext->flushAndSubmit(m_SkSurface.get());

		CanvasEnd(m_CurveMaskCanvas);
		CanvasEnd(m_Canvas);
	}

	GrBackendRenderTarget SkiaWidget::CreateRenderTarget(int w, int h)
	{
		auto surfaceFormat = QOpenGLContext::currentContext()->format();

		GrGLFramebufferInfo fbInfo;
		fbInfo.fFBOID = defaultFramebufferObject();
		fbInfo.fFormat = GL_RGBA8;

		return GrBackendRenderTargets::MakeGL(w, h, surfaceFormat.samples(), surfaceFormat.stencilBufferSize(), fbInfo);
	}

	sk_sp<SkSurface> SkiaWidget::CreateSurface(const GrBackendRenderTarget& target)
	{
		sk_sp<SkColorSpace> srgb = SkColorSpace::MakeSRGB();
		auto surface = SkSurfaces::WrapBackendRenderTarget(
			m_GrContext.get(),
			target,
			kBottomLeft_GrSurfaceOrigin,
			kRGBA_8888_SkColorType,
			srgb,
			nullptr
		);
		return surface;
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
		emit MouseWheelEvent(event);
	}

	void SkiaWidget::keyPressEvent(QKeyEvent* event)
	{
		emit KeyboardEvent(event);
	}

	void SkiaWidget::CanvasStart(SkCanvas* canvas, const SkColor& clearColor)
	{
		canvas->clear(clearColor);
		canvas->save();
		canvas->translate(size().width() / 2, size().height() / 2);
		canvas->scale(m_ViewportZoom, m_ViewportZoom);
		canvas->translate(-m_ViewportCenter.GetX(), -m_ViewportCenter.GetY());
	}

	void SkiaWidget::CanvasEnd(SkCanvas * canvas)
	{
		canvas->restore();
	}

	void SkiaWidget::PaintScene()
	{
		if (!m_Scene)
		{
			return;
		}

		std::set<CityDraft::DraftZSortKey<std::shared_ptr<CityDraft::Drafts::Draft>>> orderedVisibleDrafts;

		auto vieportBox = GetViewportBox();
		size_t draftsInViewportNum = m_Scene->QueryDrafts(vieportBox, CityDraft::Scene::QueryParams(), orderedVisibleDrafts);

		m_WidgetLogger->trace("{} drafts inside the viewport box: [({},{}), ({},{})]", draftsInViewportNum,
			vieportBox.GetMin().GetX(),
			vieportBox.GetMin().GetY(),
			vieportBox.GetMax().GetX(),
			vieportBox.GetMax().GetY()
			);

		SkCanvas* canvas = m_SkSurface->getCanvas();
		BOOST_ASSERT(canvas);

		for (const auto& draft : orderedVisibleDrafts)
		{
			auto renderProxy = draft.Draft->GetRenderProxy();
			auto painter = std::dynamic_pointer_cast<SkiaPainters::Painter>(renderProxy);
			if(!painter)
			{
				painter = CreatePainter(draft.Draft.get(), draft.Draft->GetTransform());
				painter->SetOwner(draft.Draft.get());
				draft.Draft->SetRenderProxy(painter);
			}
			PaintOrQueue(painter);
		}
	}

	void SkiaWidget::PaintOrQueue(std::shared_ptr<SkiaPainters::Painter> painter)
	{
		if (m_IsGlPainting)
		{
			painter->Paint(this, m_Canvas);
		}
		else
		{
			m_QueuedPainters.push(painter);
		}
	}

	std::shared_ptr<SkiaPainters::Painter> SkiaWidget::CreatePainter(CityDraft::Drafts::Draft* draft, const Transform2D& transform)
	{
		if(auto* imageAsset = dynamic_cast<CityDraft::Assets::SkiaImage*>(draft->GetAsset()))
		{
			return std::make_shared<SkiaPainters::Image>(imageAsset, transform);
		}
		else if(auto* colorCurve = dynamic_cast<CityDraft::Drafts::SkiaColorCurve*>(draft))
		{
			return std::make_shared<SkiaPainters::ColorCurve>(colorCurve, m_CurveMaskCanvas);
		}

		BOOST_ASSERT(false);
		return nullptr;
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
