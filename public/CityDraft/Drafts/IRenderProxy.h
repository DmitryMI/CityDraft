#pragma once

namespace CityDraft::Drafts
{
	class Draft;

	class IRenderProxy
	{
	public:
		virtual Draft* GetOwner() const = 0;
	};
}