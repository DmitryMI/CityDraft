#pragma once



#include "CityDraft/Assets/SkiaImage.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Input/IKeyBindingProvider.h"
#include "CityDraft/Scene.h"
#include "CityDraft/Vector2D.h"
#include "include/core/SkSurface.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "IRenderer.h"
#include <QMouseEvent>
#include <qopenglextrafunctions.h>
#include <qopenglwidget.h>
#include <QWheelEvent>
#include <spdlog/spdlog.h>
#include "SkiaPainters/Painter.h"
#include <queue>

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

		// IRenderer
		std::shared_ptr<CityDraft::Scene> GetScene() const override;
		void Paint(CityDraft::Assets::Asset* asset, const Transform2D& transform) override;
		void PaintRect(const QPointF& pixelMin, const QPointF& pixelMax, const QColor& color, double thickness) override;
		void PaintRect(const Vector2D& min, const Vector2D& max, const QColor& color, double thickness) override;
		const Vector2D GetViewportCenter() const override;
		double GetViewportZoom() const override;
		void SetViewportTransform(const Vector2D& center, double zoom) override;
		void Repaint() override;
		Vector2D Project(const QPointF& pixelCoord) const override;


	signals:
		void GraphicsInitialized(SkiaWidget* source);
		void GraphicsPainting(SkiaWidget* source);
		void MouseButtonEvent(QMouseEvent* event, bool pressed);
		void MouseMoveEvent(QMouseEvent* event);


	protected:

		// OpenGL
		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

		// Input
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;

		// Drawing
		void PaintScene();
		void Paint(CityDraft::Assets::SkiaImage* image, const Transform2D& transform);
		void PaintOrQueue(std::shared_ptr<SkiaPainters::Painter> painter);

		// Utility
		Vector2D GetViewportProjectedSize() const;
	private:

		// Skia
		sk_sp<const GrGLInterface> m_GrInterface;
		sk_sp<GrDirectContext> m_GrContext;
		sk_sp<SkSurface> m_SkSurface;
		SkCanvas* m_Canvas;
		GrBackendRenderTarget m_BackendRenderTarget;
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
		std::vector<std::shared_ptr<Drafts::Draft>> m_ViewportDraftsBuffer;

		// State
		bool m_IsGlPainting = false;

		AxisAlignedBoundingBox2D GetViewportBox() const;
		void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
		static void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}
