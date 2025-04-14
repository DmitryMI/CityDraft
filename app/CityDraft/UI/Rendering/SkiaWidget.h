#pragma once

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <spdlog/spdlog.h>
#include <string_view>
#include <mutex>
#include "OpenGlUtils.h"
#include <QMouseEvent>
#include "CityDraft/Vector2D.h"
#include "CityDraft/AxisAlignedBoundingBox2D.h"
#include "CityDraft/Scene.h"

namespace CityDraft::UI::Rendering
{
	class SkiaWidget : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		SkiaWidget(QWidget* parent = nullptr);

		sk_sp<GrDirectContext> GetDirectContext() const;
		QOpenGLExtraFunctions& GetGlFunctions();

		std::shared_ptr<CityDraft::Scene> GetScene() const;
		void SetScene(std::shared_ptr<CityDraft::Scene> scene);

	signals:
		void GraphicsInitialized(SkiaWidget* source);

	protected:
		void initializeGL() override;
		void resizeGL(int w, int h) override;
		void paintGL() override;

		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;

		virtual void PaintScene();

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

		AxisAlignedBoundingBox2D GetViewportBox() const;
		void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
		static void GlLogCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}
