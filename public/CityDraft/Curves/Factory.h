#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>

namespace CityDraft::Curves
{
	class ICurve;

	class Factory
	{
	public:
		using Constructor = std::function<std::shared_ptr<ICurve>()>;

		std::shared_ptr<ICurve> Create(const std::string& curveType);
		void RegisterCurveType(std::string name, Constructor constructor);

		static Factory& GetInstance();
	private:
		std::unordered_map<std::string, Constructor> m_CurveTypes;

		static std::unique_ptr<Factory> Instance;

	};

	#define REGISTER_CURVE_TYPE(classname) \
	public:	\
		inline std::string GetCurveTypeName() const override {return boost::typeindex::type_id<classname>().pretty_name();} \
	private:\
        struct classname##Registrator { \
            classname##Registrator() { \
                Factory::GetInstance().RegisterCurveType(boost::typeindex::type_id<classname>().pretty_name(), []() { return std::make_shared<classname>(); }); \
            } \
        }; \
        static inline classname##Registrator global_##classname##Registrator{};
}