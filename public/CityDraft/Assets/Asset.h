#pragma once

#include "CityDraft/DraftObject.h"
#include <boost/url.hpp>

namespace CityDraft::Assets
{
	enum class AssetStatus
	{
		Initialized,
		Loading,
		Loaded,
		LoadingFailed
	};

	class Asset
	{
	public:
		inline Asset() :
			m_AssetUrl("")
		{

		}

		inline Asset(const boost::url& url):
			m_AssetUrl(url)
		{

		}

		virtual DraftObject* CreateDraftObject() = 0;
		virtual AssetStatus GetStatus() const = 0;

		inline const boost::url& GetUrl() const
		{
			return m_AssetUrl;
		}

	protected:
		boost::url m_AssetUrl;
	};
}
