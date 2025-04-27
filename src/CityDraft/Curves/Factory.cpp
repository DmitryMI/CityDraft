#pragma once
#include "CityDraft/Curves/Factory.h"
#include <boost/assert.hpp>


namespace CityDraft::Curves
{
	std::unique_ptr<Factory> Factory::Instance = nullptr;
	
	void Factory::RegisterCurveType(std::string name, Constructor constructor)
	{
		BOOST_ASSERT(!m_CurveTypes.contains(name));
		m_CurveTypes[name] = constructor;
	}

	std::shared_ptr<ICurve> Factory::Create(const std::string& curveType)
	{
		BOOST_ASSERT(m_CurveTypes.contains(curveType));
		return m_CurveTypes[curveType]();
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