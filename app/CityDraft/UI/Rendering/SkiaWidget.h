#pragma once



#include "include/core/SkSurface.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include <qopenglwidget.h>
#include <qopenglextrafunctions.h>
#include <spdlog/spdlog.h>
#include <QMouseEvent>
#include "CityDraft/Vector2D.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Scene.h"
#include "CityDraft/Drafts/SkiaImage.h"
#include <QWheelEvent>

namespace CityDraft::UI::Rendering
{
	enum class MouseAction
	{
		NoAction,
		DraftDragging,
		Selection,
		ViewportPanning
	};

	class SkiaWidget : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		SkiaWidget(QWidget* parent = nullptr);

		sk_sp<GrDirectContext> GetDirectContext() const;
		QOpenGLExtraFunctions& GetGlFunctions();

		std::shared_ptr<CityDraft::Scene> GetScene() const;
		void SetScene(std::shared_ptr<CityDraft::Scene> scene);

		QPointF GetCursorProjectedPosition() const;

	signals:
		void GraphicsInitialized(SkiaWidget* source);
		void CursorPositionChanged(const QPointF& projectedPosition);

	protected:
		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;

		virtual void PaintScene();
		virtual void PaintSkiaImage(SkCanvas* canvas, CityDraft::Drafts::SkiaImage* image);

	private:
		sk_sp<const GrGLInterface> m_GrInterface;
		sk_sp<GrDirectContext> m_GrContext;
		sk_sp<SkSurface> m_SkSurface;
		GrBackendRenderTarget m_BackendRenderTarget;
		QOpenGLExtraFunctions m_GlFuncs;

		std::shared_ptr<spdlog::logger> m_WidgetLogger;
		std::shared_ptr<spdlog::logger> m_SkiaLogger;
		std::shared_ptr<spdlog::logger> m_GlLogger;
		std::shared_ptr<CityDraft::Scene> m_Scene;

		double m_ViewportZoom = 1.0;
		Vector2D m_ViewportTranslation{ 0,0 };
		std::vector<std::shared_ptr<Drafts::Draft>> m_ViewportDraftsBuffer;
		QPointF m_CursorProjectedPosition;

		AxisAlignedBoundingBox2D GetViewportBox() const;
		void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
		static void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}
