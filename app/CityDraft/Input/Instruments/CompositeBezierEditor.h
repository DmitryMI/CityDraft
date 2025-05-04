#pragma once

#include "Instrument.h"

namespace CityDraft::Input::Instruments
{
	class CompositeBezierEditor: public Instrument
	{
	public:

		constexpr static int Priority = 10;

		CompositeBezierEditor(const Dependencies& dependencies);
		virtual ~CompositeBezierEditor();
		int GetPriority() const override
		{
			return Priority;
		}

		inline QString GetName() const override
		{
			return "Composite Bezier Edit";
		}

		void OnPaint() override;
		EventChainAction OnRendererMouseButton(QMouseEvent* event, bool pressed) override;
		EventChainAction OnRendererMouseMove(QMouseEvent* event) override;
		EventChainAction OnRendererMouseWheel(QWheelEvent* event) override;

		void QueryTools(std::map<ToolDescryptor, QString>& toolDescriptions) override;

	protected:
		inline std::shared_ptr<spdlog::logger> GetLogger() override
		{
			return CityDraft::Logging::LogManager::CreateLogger("CompositeBezierEditor");
		} ;
	};
}
