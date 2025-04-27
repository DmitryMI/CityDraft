#pragma once
#include "CityDraft/Curves/Factory.h"
#include <boost/assert.hpp>


namespace CityDraft::Curves
{
	std::unique_ptr<Factory> Factory::Instance = nullptr;
	
	void Factory::RegisterType(std::string name, CurveConstructor constructor)
	{
		BOOST_ASSERT(!m_CurveTypes.contains(name));
		m_CurveTypes[name] = constructor;
	}

	void Factory::RegisterType(std::string name, WidthProviderConstructor constructor)
	{
		BOOST_ASSERT(!m_WidthProviderTypes.contains(name));
		m_WidthProviderTypes[name] = constructor;
	}

	std::shared_ptr<ICurve> Factory::CreateCurve(const std::string& curveType)
	{
		BOOST_ASSERT(m_CurveTypes.contains(curveType));
		return m_CurveTypes.at(curveType)();
	}

	std::shared_ptr<IWidthProvider> Factory::CreateWidthProvider(const std::string& widthProviderType)
	{
		BOOST_ASSERT(m_WidthProviderTypes.contains(widthProviderType));
		return m_WidthProviderTypes.at(widthProviderType)();
	}

	Factory& Factory::GetInstance()
	{
		if(!Instance)
		{
			Instance = std::make_unique<Factory>();
		}

		return *Instance;
	}
}