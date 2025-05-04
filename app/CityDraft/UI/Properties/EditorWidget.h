#pragma once

#include <QWidget>
#include <optional>
#include <qlineedit.h>
#include "CityDraft/Drafts/Properties/TypedProperty.h"

namespace CityDraft::UI::Properties
{
	enum class PropertyFlags: uint8_t
	{
		Readable = (1 << 0),
		Writable = (1 << 1),
		MultipleValues = (1 << 2),
		Error = (1 << 3),
	};

	constexpr PropertyFlags operator|(PropertyFlags a, PropertyFlags b)
	{
		return static_cast<PropertyFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	constexpr PropertyFlags operator&(PropertyFlags a, PropertyFlags b)
	{
		return static_cast<PropertyFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}

	constexpr bool HasFlag(PropertyFlags a, PropertyFlags b)
	{
		return (a & b) == b;
	}

	class EditorWidget: public QWidget
	{
		Q_OBJECT

	public:
		EditorWidget(QWidget* parent = nullptr);
		~EditorWidget() = default;
		virtual std::string_view GetPropertyName() const = 0;

	protected:

		void MarkLineEdit(QLineEdit* lineEdit, PropertyFlags flags) const;

		template<typename T>
		std::optional<T> GetCurrentValue(const CityDraft::Drafts::Properties::TypedPropertyGroup<T>& group) const
		{
			const T& firstValue = group[0]->Get();
			for(size_t i = 1; i < group.size(); i++)
			{
				const T& value = group[i]->Get();
				if(value != firstValue)
				{
					return std::nullopt;
				}
			}

			return firstValue;
		}

		template<typename T>
		std::optional<T> GetCurrentValue(const CityDraft::Drafts::Properties::Vector& group) const
		{
			const T& firstValue = std::any_cast<T>(group[0]->GetAny());
			for(size_t i = 1; i < group.size(); i++)
			{
				const T& value = std::any_cast<T>(group[i]->GetAny());
				if(value != firstValue)
				{
					return std::nullopt;
				}
			}

			return firstValue;
		}
	};
}