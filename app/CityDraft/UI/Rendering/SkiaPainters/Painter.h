#pragma once

#include "Painter.h"
#include "include/core/SkCanvas.h"
#include "CityDraft/Drafts/IRenderProxy.h"
#include "CityDraft/Drafts/Draft.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Painter : public CityDraft::Drafts::IRenderProxy
	{
	public:
		virtual void Paint(SkCanvas* canvas) = 0;

		CityDraft::Drafts::Draft* GetOwner() const override
		{
			return m_Owner;
		}

		inline void SetOwner(CityDraft::Drafts::Draft* owner)
		{
			m_Owner = owner;
		}

	protected:
		CityDraft::Drafts::Draft* m_Owner = nullptr;
	};
}
