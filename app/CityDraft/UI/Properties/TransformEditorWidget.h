#pragma once

#include <QWidget>
#include <QLineEdit>
#include "EditorWidget.h"
#include "ui_TransformEditorWidget.h"
#include "CityDraft/Drafts/Properties/TypedProperty.h"
#include "CityDraft/Transform2D.h"

namespace CityDraft::UI::Properties
{
	using TransformPropertyGroup = CityDraft::Drafts::Properties::TypedPropertyGroup<CityDraft::Transform2D>;

	struct TransformValue
	{
		std::optional<double> TranslationX;
		std::optional<double> TranslationY;
		std::optional<CityDraft::Radians> Rotation;
		std::optional<double> ScaleX;
		std::optional<double> ScaleY;
	};

	class TransformEditorWidget: public EditorWidget
	{
		Q_OBJECT

	public:
		TransformEditorWidget(const TransformPropertyGroup& properties, QWidget* parent = nullptr);
		~TransformEditorWidget();

		inline std::string_view GetPropertyName() const override
		{
			BOOST_ASSERT(m_Properties.size() > 0);
			return m_Properties[0]->GetName();
		}

	private slots:
		void OnEditingFinished(QLineEdit* lineEdit);

	private:
		Ui::TransformEditorWidgetClass m_Ui;
		TransformPropertyGroup m_Properties;
		std::vector<boost::signals2::connection> m_ValueChangedConnections;

		TransformValue GetCommonCurrentValues();

		void UpdateUi();
		void OnValueChanged();

		void SetRotation(const CityDraft::Radians& rotation);
		void SetTranslationX(double value);
		void SetTranslationY(double value);
		void SetScaleX(double value);
		void SetScaleY(double value);

		template<typename Setter>
		void ChangeTransform(Setter setter)
		{
			std::vector<boost::signals2::shared_connection_block> blockers;
			for(auto& conn : m_ValueChangedConnections)
			{
				blockers.emplace_back(conn, true);
			}

			for(const auto& property : m_Properties)
			{
				auto transform = property->Get();
				setter(transform);
				property->Set(transform);
			}

			blockers.clear();
			UpdateUi();
		}
	};
}