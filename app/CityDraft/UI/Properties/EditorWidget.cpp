#include "EditorWidget.h"

namespace CityDraft::UI::Properties
{
	EditorWidget::EditorWidget(QWidget* parent):
		QWidget(parent)
	{}

	void EditorWidget::MarkLineEdit(QLineEdit * lineEdit, PropertyFlags flags) const
	{
		BOOST_ASSERT(HasFlag(flags, PropertyFlags::Readable) || HasFlag(flags, PropertyFlags::Writable));
		BOOST_ASSERT(!HasFlag(flags, PropertyFlags::MultipleValues) || HasFlag(flags, PropertyFlags::Readable));
		BOOST_ASSERT(!HasFlag(flags, PropertyFlags::Error) || HasFlag(flags, PropertyFlags::Writable));

		lineEdit->setPlaceholderText("");
		lineEdit->setStyleSheet("");

		bool isReadOnly = !HasFlag(flags, PropertyFlags::Writable);
		lineEdit->setReadOnly(isReadOnly);
		if (isReadOnly)
		{
			lineEdit->setStyleSheet("QLineEdit { background-color: lightgray; }");
		}

		if (HasFlag(flags, PropertyFlags::Error))
		{
			lineEdit->setText("");
			lineEdit->setPlaceholderText("<user input malformed>");
			lineEdit->setStyleSheet("QLineEdit { background-color: red; }");
			return;
		}

		if (!HasFlag(flags, PropertyFlags::Readable))
		{
			lineEdit->setPlaceholderText("<write only property>");
			return;
		}

		if (HasFlag(flags, PropertyFlags::MultipleValues))
		{
			lineEdit->setPlaceholderText("<multiple values>");
			return;
		}
	}

}