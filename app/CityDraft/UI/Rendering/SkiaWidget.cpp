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

// Must be included the latest, or else does not compile
#include <GL/gl.h>

struct ZSortKey
{
	int64_t LayerZ;
	int64_t DraftZ;
	CityDraft::Drafts::Draft* Draft;

	ZSortKey(CityDraft::Drafts::Draft* draft)
	{
		LayerZ = draft->GetLayer()->GetZOrder();
		DraftZ = draft->GetZOrder();
		Draft = draft;
	}

	bool operator<(const ZSortKey& other) const
	{
		if (LayerZ != other.LayerZ)
			return LayerZ < other.LayerZ;
		if (DraftZ != other.DraftZ)
			return DraftZ < other.DraftZ;
		return Draft < other.Draft;
	}
};

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

		m_IsGlPainting = true;

		m_GrContext->resetContext(kAll_GrBackendState);

		m_Canvas->clear(SK_ColorTRANSPARENT);
		m_Canvas->save();
		m_Canvas->translate(size().width() / 2, size().height() / 2);
		m_Canvas->scale(m_ViewportZoom, m_ViewportZoom);
		m_Canvas->translate(-m_ViewportCenter.GetX(), -m_ViewportCenter.GetY());

		PaintScene();

		while (!m_QueuedPainters.empty())
		{
			auto painter = m_QueuedPainters.front();
			m_QueuedPainters.pop();
			painter->Paint(m_Canvas);
		}

		emit GraphicsPainting(this);

		m_IsGlPainting = false;
		m_GrContext->flushAndSubmit(m_SkSurface.get());
		m_Canvas->restore();
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

	void SkiaWidget::PaintScene()
	{
		if (!m_Scene)
		{
			return;
		}

		m_ViewportDraftsBuffer.clear();

		auto vieportBox = GetViewportBox();
		size_t draftsInViewportNum = m_Scene->QueryDraftsOnAllLayers(vieportBox, m_ViewportDraftsBuffer);

		m_WidgetLogger->trace("{} drafts inside the viewport box: [({},{}), ({},{})]", draftsInViewportNum,
			vieportBox.GetMin().GetX(),
			vieportBox.GetMin().GetY(),
			vieportBox.GetMax().GetX(),
			vieportBox.GetMax().GetY()
			);

		std::set<ZSortKey> orderedVisibleDrafts;
		for (const auto& draftPtr : m_ViewportDraftsBuffer)
		{
			if (draftPtr->GetLayer()->IsVisible())
			{
				orderedVisibleDrafts.emplace(draftPtr.get());
			}
		}

		SkCanvas* canvas = m_SkSurface->getCanvas();
		BOOST_ASSERT(canvas);

		for (const auto& draft : orderedVisibleDrafts)
		{
			auto renderProxy = draft.second->GetRenderProxy();
			auto painter = std::dynamic_pointer_cast<SkiaPainters::Painter>(renderProxy);
			if(!painter)
			{
				painter = CreatePainter(draft.second,draft.second->GetTransform());
				painter->SetOwner(draft.second);
				draft.second->SetRenderProxy(painter);
			}
			PaintOrQueue(painter);
		}
	}

	void SkiaWidget::PaintOrQueue(std::shared_ptr<SkiaPainters::Painter> painter)
	{
		if (m_IsGlPainting)
		{
			painter->Paint(m_Canvas);
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
			return std::make_shared<SkiaPainters::ColorCurve>(colorCurve);
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
