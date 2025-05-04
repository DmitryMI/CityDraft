#include <gtest/gtest.h>
#include <memory>
#include "CityDraft/Drafts/Properties/Property.h"
#include "CityDraft/Drafts/Properties/View.h"
#include "CityDraft/Drafts/DraftMock.h"
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/Assets/AssetMock.h"

namespace CityDraft::Drafts::Properties
{
	TEST(PropertiesTest, FindCommonPropertiesTest)
	{
		CityDraft::Assets::AssetMock asset;
		
		CityDraft::Drafts::DraftMock draft1(&asset);
		CityDraft::Drafts::DraftMock draft2(&asset);
		CityDraft::Drafts::DraftMock draft3(&asset);

		auto viewA1 = std::make_shared<View<int>>("ViewA", &draft1, nullptr, nullptr, nullptr);
		auto viewA2 = std::make_shared<View<int>>("ViewA", &draft2, nullptr, nullptr, nullptr);

		auto viewB1 = std::make_shared<View<int>>("ViewB", &draft1, nullptr, nullptr, nullptr);
		auto viewB2 = std::make_shared<View<int>>("ViewB", &draft2, nullptr, nullptr, nullptr);
		auto viewB3 = std::make_shared<View<int>>("ViewB", &draft3, nullptr, nullptr, nullptr);

		auto viewC2 = std::make_shared<View<int>>("ViewC", &draft2, nullptr, nullptr, nullptr);
		auto viewC3 = std::make_shared<View<int>>("ViewC", &draft3, nullptr, nullptr, nullptr);

		auto viewD1 = std::make_shared<View<int>>("ViewD", &draft1, nullptr, nullptr, nullptr);
		auto viewD2 = std::make_shared<View<int>>("ViewD", &draft2, nullptr, nullptr, nullptr);
		auto viewD3 = std::make_shared<View<int>>("ViewD", &draft3, nullptr, nullptr, nullptr);

		auto viewE1 = std::make_shared<View<int>>("ViewE", &draft1, nullptr, nullptr, nullptr);

		Set set1 = Set{viewA1, viewB1, viewD1, viewE1};
		Set set2 = Set{viewA2, viewB2, viewD2};
		Set set3 = Set{viewB3, viewC3, viewD3};

		Map commonProps = FindCommonProperties({set1, set2, set3});
		ASSERT_EQ(commonProps.size(), 2);
		ASSERT_FALSE(commonProps.contains("ViewA"));
		ASSERT_TRUE(commonProps.contains("ViewB"));
		ASSERT_FALSE(commonProps.contains("ViewC"));
		ASSERT_TRUE(commonProps.contains("ViewD"));
		ASSERT_FALSE(commonProps.contains("ViewE"));

		auto names = std::vector<std::string_view>{"ViewB", "ViewD"};
		for(const auto& name : names)
		{
			const auto& vec = commonProps.at(name);
			for(size_t propI = 0; propI < vec.size(); propI++)
			{
				const auto& prop = vec[propI];
				ASSERT_EQ(prop->GetName(), name);
				if(propI > 0)
				{
					ASSERT_NE(prop->GetOwner(), vec[propI - 1]->GetOwner());
				}
			}
		}
	}

	TEST(PropertiesTest, PolymorphismTest)
	{
		CityDraft::Assets::AssetMock asset;

		CityDraft::Drafts::DraftMock draft1(&asset);

		auto viewA = std::make_shared<View<int>>("ViewA", &draft1, nullptr, nullptr, nullptr);
		auto typedPropertyA = dynamic_pointer_cast<TypedProperty<int>>(viewA);
		ASSERT_TRUE(typedPropertyA);
		auto propertyA = dynamic_pointer_cast<Property>(typedPropertyA);
		ASSERT_TRUE(propertyA);
		viewA = dynamic_pointer_cast<View<int>>(propertyA);
		ASSERT_TRUE(viewA);

		auto viewB = std::make_shared<View<std::string>>("ViewB", &draft1, nullptr, nullptr, nullptr);
		auto typedPropertyB = dynamic_pointer_cast<TypedProperty<std::string>>(viewB);
		ASSERT_TRUE(typedPropertyB);
		auto propertyB = dynamic_pointer_cast<Property>(typedPropertyB);
		ASSERT_TRUE(propertyB);
		viewB = dynamic_pointer_cast<View<std::string>>(propertyB);
		ASSERT_TRUE(viewB);
	}
}
