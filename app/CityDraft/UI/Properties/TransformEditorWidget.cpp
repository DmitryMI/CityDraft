#include "TransformEditorWidget.h"
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDoubleValidator>


namespace CityDraft::UI::Properties
{

	TransformEditorWidget::TransformEditorWidget(const TransformPropertyGroup& properties, QWidget* parent)
		: EditorWidget(parent),
		m_Properties(properties)
	{
		m_Ui.setupUi(this);

		BOOST_ASSERT(m_Properties.size() > 0);
		m_Ui.groupBox->setTitle(QString::fromStdString(std::string(m_Properties[0]->GetName())));

		for(const auto& property : m_Properties)
		{
			m_ValueChangedConnections.push_back(property->ConnectToValueChanged(std::bind(&TransformEditorWidget::OnValueChanged, this)));
		}

		QRegularExpression angleRegex(R"(^-?\d+(\.\d+)?(deg|rad)?$)");
		QValidator* angleValidator = new QRegularExpressionValidator(angleRegex, this);
		m_Ui.RotationEdit->setValidator(angleValidator);

		QRegularExpression translationRegex(R"(^-?\d+(\.\d+)?(m)?$)");
		QValidator* translationValidator = new QRegularExpressionValidator(translationRegex, this);
		m_Ui.TranslationXEdit->setValidator(translationValidator);
		m_Ui.TranslationYEdit->setValidator(translationValidator);

		QDoubleValidator* scaleValidator = new QDoubleValidator(0.001, 10000, 2, this);
		m_Ui.ScaleXEdit->setValidator(scaleValidator);
		m_Ui.ScaleYEdit->setValidator(scaleValidator);

		connect(m_Ui.RotationEdit, &QLineEdit::editingFinished, this, [this]() {
			OnEditingFinished(m_Ui.RotationEdit);
		});
		connect(m_Ui.TranslationXEdit, &QLineEdit::editingFinished, this, [this]() {
			OnEditingFinished(m_Ui.TranslationXEdit);
		});
		connect(m_Ui.TranslationYEdit, &QLineEdit::editingFinished, this, [this]() {
			OnEditingFinished(m_Ui.TranslationYEdit);
		});
		connect(m_Ui.ScaleXEdit, &QLineEdit::editingFinished, this, [this]() {
			OnEditingFinished(m_Ui.ScaleXEdit);
		});
		connect(m_Ui.ScaleYEdit, &QLineEdit::editingFinished, this, [this]() {
			OnEditingFinished(m_Ui.ScaleYEdit);
		});

		UpdateUi();
	}

	TransformEditorWidget::~TransformEditorWidget()
	{
		for(const auto& con : m_ValueChangedConnections)
		{
			con.disconnect();
		}
	}

	void TransformEditorWidget::OnEditingFinished(QLineEdit* lineEdit)
	{
		bool isMeter = false;
		bool isDeg = false;
		bool parseOk = true;
		QString text = lineEdit->text();
		QString degPostfix = "deg";
		QString radPostfix = "rad";
		QString meterPostfix = "m";
		if(text.endsWith(degPostfix))
		{
			text.remove(text.length() - degPostfix.length(), degPostfix.length());
			isDeg = true;
		}
		else if(text.endsWith(radPostfix))
		{
			text.remove(text.length() - radPostfix.length(), radPostfix.length());
			isDeg = false;
		}
		else if(text.endsWith(meterPostfix))
		{
			text.remove(text.length() - meterPostfix.length(), meterPostfix.length());
			isMeter = true;
		}

		double userValue = text.toDouble(&parseOk);
		if(!parseOk)
		{
			MarkLineEdit(lineEdit, PropertyFlags::Error);
			return;
		}

		if(lineEdit == m_Ui.RotationEdit)
		{
			if(isDeg)
			{
				SetRotation(CityDraft::Degrees(userValue));
			}
			else
			{
				SetRotation(CityDraft::Radians(userValue));
			}
			
			return;
		}
		else if(lineEdit == m_Ui.TranslationXEdit)
		{
			// TODO Meters
			SetTranslationX(userValue);
		}
		else if(lineEdit == m_Ui.TranslationYEdit)
		{
			SetTranslationY(userValue);
		}
		else if(lineEdit == m_Ui.ScaleXEdit)
		{
			SetScaleX(userValue);
		}
		else if(lineEdit == m_Ui.ScaleYEdit)
		{
			SetScaleY(userValue);
		}
	}

	TransformValue TransformEditorWidget::GetCommonCurrentValues()
	{
		TransformValue commonValues;
		Transform2D firstTransform = m_Properties[0]->Get();
		commonValues.TranslationX = firstTransform.Translation.GetX();
		commonValues.TranslationY = firstTransform.Translation.GetY();
		commonValues.Rotation = firstTransform.Rotation;
		commonValues.ScaleX = firstTransform.Scale.GetX();
		commonValues.ScaleY = firstTransform.Scale.GetY();

		size_t commonValuesLeft = 5;
		for(size_t i = 1; i < m_Properties.size(); i++)
		{
			Transform2D transform = m_Properties[i]->Get();
			if(commonValues.TranslationX.has_value() && commonValues.TranslationX.value() != transform.Translation.GetX())
			{
				commonValues.TranslationX = std::nullopt;
				commonValuesLeft--;
			}
			if(commonValues.TranslationY.has_value() && commonValues.TranslationY.value() != transform.Translation.GetY())
			{
				commonValues.TranslationY = std::nullopt;
				commonValuesLeft--;
			}
			if(commonValues.Rotation.has_value() && commonValues.Rotation.value() != transform.Rotation)
			{
				commonValues.Rotation = std::nullopt;
				commonValuesLeft--;
			}
			if(commonValues.ScaleX.has_value() && commonValues.ScaleX.value() != transform.Scale.GetX())
			{
				commonValues.TranslationX = std::nullopt;
				commonValuesLeft--;
			}
			if(commonValues.ScaleY.has_value() && commonValues.ScaleY.value() != transform.Scale.GetY())
			{
				commonValues.ScaleY = std::nullopt;
				commonValuesLeft--;
			}

			if(commonValuesLeft == 0)
			{
				break;
			}
		}

		return commonValues;
	}

	void TransformEditorWidget::UpdateUi()
	{
		PropertyFlags flags{0};
		if(m_Properties[0]->IsWritable())
		{
			flags = flags | PropertyFlags::Writable;
		}

		PropertyFlags translationXFlags = flags;
		PropertyFlags translationYFlags = flags;
		PropertyFlags rotationFlags = flags;
		PropertyFlags scaleXFlags = flags;
		PropertyFlags scaleYFlags = flags;

		if(m_Properties[0]->IsReadable())
		{
			flags = flags | PropertyFlags::Readable;
			translationXFlags = flags;
			translationYFlags = flags;
			rotationFlags = flags;
			scaleXFlags = flags;
			scaleYFlags = flags;

			TransformValue currentValue = GetCommonCurrentValues();

			if(!currentValue.TranslationX)
			{
				translationXFlags = translationXFlags | PropertyFlags::MultipleValues;
			}
			else
			{
				m_Ui.TranslationXEdit->setText(QString::number(currentValue.TranslationX.value()));
			}
			if(!currentValue.TranslationY)
			{
				translationYFlags = translationYFlags | PropertyFlags::MultipleValues;
			}
			else
			{
				m_Ui.TranslationYEdit->setText(QString::number(currentValue.TranslationY.value()));
			}
			if(!currentValue.Rotation)
			{
				rotationFlags = rotationFlags | PropertyFlags::MultipleValues;
			}
			else
			{
				m_Ui.RotationEdit->setText(QString::number(Degrees(currentValue.Rotation.value()).Value) + "deg");
			}
			if(!currentValue.ScaleX)
			{
				scaleXFlags = scaleXFlags | PropertyFlags::MultipleValues;
			}
			else
			{
				m_Ui.ScaleXEdit->setText(QString::number(currentValue.ScaleX.value()));
			}
			if(!currentValue.ScaleY)
			{
				scaleYFlags = scaleYFlags | PropertyFlags::MultipleValues;
			}
			else
			{
				m_Ui.ScaleYEdit->setText(QString::number(currentValue.ScaleY.value()));
			}
		}

		MarkLineEdit(m_Ui.TranslationXEdit, translationXFlags);
		MarkLineEdit(m_Ui.TranslationYEdit, translationYFlags);
		MarkLineEdit(m_Ui.RotationEdit, rotationFlags);
		MarkLineEdit(m_Ui.ScaleXEdit, scaleXFlags);
		MarkLineEdit(m_Ui.ScaleYEdit, scaleYFlags);
	}

	void TransformEditorWidget::OnValueChanged()
	{
		UpdateUi();
	}

	void TransformEditorWidget::SetRotation(const CityDraft::Radians& rotation)
	{
		auto setter = [rotation](Transform2D& transform) {
			transform.Rotation = rotation;
		};
		ChangeTransform(setter);
	}

	void TransformEditorWidget::SetTranslationX(double value)
	{
		auto setter = [value](Transform2D& transform) {
			transform.Translation.SetX(value);
		};
		ChangeTransform(setter);
	}

	void TransformEditorWidget::SetTranslationY(double value)
	{
		auto setter = [value](Transform2D& transform) {
			transform.Translation.SetY(value);
		};
		ChangeTransform(setter);
	}

	void TransformEditorWidget::SetScaleX(double value)
	{
		auto setter = [value](Transform2D& transform) {
			transform.Scale.SetX(value);
		};
		ChangeTransform(setter);
	}

	void TransformEditorWidget::SetScaleY(double value)
	{
		auto setter = [value](Transform2D& transform) {
			transform.Scale.SetY(value);
		};
		ChangeTransform(setter);
	}

}