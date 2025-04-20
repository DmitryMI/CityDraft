#pragma once

#include <array>

namespace CityDraft
{
	template<typename T>
	struct LinearColor
	{
		std::array<T, 4> RGBA{ 0 };

		LinearColor() :
			RGBA{ 0, 0, 0, 0 }
		{

		}

		template<typename T>
		LinearColor(T* components, size_t channels)
		{
			for (size_t i = 0; i < channels; i++)
			{
				RGBA[i] = static_cast<float>(components[i]) / std::numeric_limits<T>::max();
			}
		}

		LinearColor(const T& r, const T& g, const T& b, const T& a) :
			RGBA{ r, g, b, a }
		{

		}

		T& R() { return RGBA[0]; }
		T& G() { return RGBA[1]; }
		T& B() { return RGBA[2]; }
		T& A() { return RGBA[3]; }

		const T& R() const { return RGBA[0]; } 
		const T& G() const { return RGBA[1]; } 
		const T& B() const { return RGBA[2]; } 
		const T& A() const { return RGBA[3]; } 
	};

	using LinearColorF = LinearColor<float>;
}
