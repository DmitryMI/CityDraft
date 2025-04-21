#pragma once

#include <string>
#include <QMouseEvent>
#include <QKeyEvent>

namespace CityDraft::Input
{
	class Utils
	{
	public:

		static std::string ToString(const Qt::MouseButton& button);
		static std::string ToString(const Qt::KeyboardModifier& modifier);
		static std::string ToString(Qt::Key key);
	};
}
