#pragma once


#include <include/core/SkCanvas.h>
#include <include/core/SkRefCnt.h>
#include <memory>
#include <qevent.h>
#include <include/core/SkSurface.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <qopenglextrafunctions.h>
#include <qopenglwidget.h>
#include <qpoint.h>
#include <qtmetamacros.h>
#include <queue>
#include <qwidget.h>
#include <spdlog/logger.h>
#include <vector>
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Drafts/Draft.h"
#include "CityDraft/Scene.h"
#include "CityDraft/Transform2D.h"
#include "CityDraft/Vector2D.h"
#include "IRenderer.h"
#include "SkiaPainters/Painter.h"

// Must be included last and in this order
#include <GL/gl.h>
#include <qopenglext.h>

namespace CityDraft::UI::Rendering
{
	enum class MouseAction
	{
		NoAction,
		DraftDragging,
		Selection,
		ViewportPanning
	};


	class SkiaWidget : public QOpenGLWidget, public IRenderer
	{
		Q_OBJECT

	public:
		SkiaWidget(QWidget* parent = nullptr);

		sk_sp<GrDirectContext> GetDirectContext() const;
		QOpenGLExtraFunctions& GetGlFunctions();

		void SetScene(std::shared_ptr<CityDraft::Scene> scene);

		SkCanvas* GetPrimaryCanvas() const;
		SkCanvas* GetMaskCanvas() const;

		// IRenderer
		std::shared_ptr<CityDraft::Scene> GetScene() const override;
		void Paint(CityDraft::Assets::Asset* asset, const Transform2D& transform) override;
		void PaintRectViewportSpace(const QPointF& pixelMin, const QPointF& pixelMax, const LinearColorF& color, double thickness) override;
		void PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& outlineColor, double outlineThickness) override;
		void PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& outlineColor, double outlineThickness, const LinearColorF& fillColor) override;
		void PaintRect(const Vector2D& min, const Vector2D& max, const LinearColorF& fillColor) override;
		void PaintCircle(const Vector2D& pos, double radius, const LinearColorF& color, double thickness) override;
		const Vector2D GetViewportCenter() const override;
		double GetViewportZoom() const override;
		void SetViewportTransform(const Vector2D& center, double zoom) override;
		void Repaint() override;
		Vector2D Project(const QPointF& pixelCoord) const override;
		QPointF Deproject(const Vector2D& pixelCoord) const override;

	signals:
		void GraphicsInitialized(SkiaWidget* source);
		void GraphicsPainting(SkiaWidget* source);
		void MouseButtonEvent(QMouseEvent* event, bool pressed);
		void MouseMoveEvent(QMouseEvent* event);
		void MouseWheelEvent(QWheelEvent* event);
		void KeyboardEvent(QKeyEvent* event);


	protected:

		// OpenGL
		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

		// Skia
		GrBackendRenderTarget CreateRenderTarget(int w, int h);
		sk_sp<SkSurface> CreateSurface(const GrBackendRenderTarget& target);

		// Input
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;

		// Drawing
		void CanvasStart(SkCanvas* canvas, const SkColor& clearColor);
		void CanvasEnd(SkCanvas* canvas);
		void PaintScene();
		void PaintOrQueue(std::shared_ptr<SkiaPainters::Painter> painter);
		std::shared_ptr<SkiaPainters::Painter> CreatePainter(CityDraft::Drafts::Draft* draft, const Transform2D& transform);


		// Utility
		Vector2D GetViewportProjectedSize() const;
	private:

		// Skia
		sk_sp<const GrGLInterface> m_GrInterface = nullptr;
		sk_sp<GrDirectContext> m_GrContext = nullptr;
		GrBackendRenderTarget m_BackendRenderTarget;
		sk_sp<SkSurface> m_SkSurface = nullptr;
		SkCanvas* m_Canvas = nullptr;
		
		GrBackendRenderTarget m_CurveMaskRenderTarget;
		sk_sp<SkSurface> m_CurveMaskSurface = nullptr;
		SkCanvas* m_CurveMaskCanvas = nullptr;

		QOpenGLExtraFunctions m_GlFuncs;
		std::queue<std::shared_ptr<SkiaPainters::Painter>> m_QueuedPainters;

		// Loggers
		std::shared_ptr<spdlog::logger> m_WidgetLogger;
		std::shared_ptr<spdlog::logger> m_SkiaLogger;
		std::shared_ptr<spdlog::logger> m_GlLogger;

		// CityDraft
		std::shared_ptr<CityDraft::Scene> m_Scene;

		// Viewport variables
		double m_ViewportZoom = 1.0;
		Vector2D m_ViewportCenter{ 0,0 };

		// State
		bool m_IsGlPainting = false;

		AxisAlignedBoundingBox2D GetViewportBox() const;
		void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
		static void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}
