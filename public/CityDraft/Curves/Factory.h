#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>

namespace CityDraft::Curves
{
	class ICurve;
	class IWidthProvider;

	class Factory
	{
	public:
		using CurveConstructor = std::function<std::shared_ptr<ICurve>()>;
		using WidthProviderConstructor = std::function<std::shared_ptr<IWidthProvider>()>;

		std::shared_ptr<ICurve> CreateCurve(const std::string& curveType);
		std::shared_ptr<IWidthProvider> CreateWidthProvider(const std::string& widthProviderType);
		void RegisterType(std::string name, CurveConstructor constructor);
		void RegisterType(std::string name, WidthProviderConstructor constructor);

		static Factory& GetInstance();
	private:
		std::unordered_map<std::string, CurveConstructor> m_CurveTypes;
		std::unordered_map<std::string, WidthProviderConstructor> m_WidthProviderTypes;

		static std::unique_ptr<Factory> Instance;

	};

	#define REGISTER_TYPE(classname) \
	public:	\
		inline std::string GetTypeName() const override {return boost::typeindex::type_id<classname>().pretty_name();} \
	private:\
        struct classname##Registrator { \
            classname##Registrator() { \
                Factory::GetInstance().RegisterType(boost::typeindex::type_id<classname>().pretty_name(), []() { return std::make_shared<classname>(); }); \
            } \
        }; \
        static inline classname##Registrator global_##classname##Registrator{};
}