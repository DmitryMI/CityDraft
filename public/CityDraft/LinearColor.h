#pragma once

#include <array>
#include <limits>
#include <stdexcept>

namespace CityDraft
{
	template<typename T>
	struct LinearColor
	{
		std::array<T, 4> RGBA{ 0 };

		constexpr LinearColor() :
			RGBA{ 0, 0, 0, 0 }
		{

		}

		template<typename TInt>
		constexpr LinearColor(TInt* components, size_t channels)
		{
			for (size_t i = 0; i < channels; i++)
			{
				RGBA[i] = static_cast<T>(components[i]) / std::numeric_limits<TInt>::max();
			}
		}

		constexpr LinearColor(const T& r, const T& g, const T& b, const T& a) :
			RGBA{ r, g, b, a }
		{

		}

		constexpr T& R() { return RGBA[0]; }
		constexpr T& G() { return RGBA[1]; }
		constexpr T& B() { return RGBA[2]; }
		constexpr T& A() { return RGBA[3]; }

		constexpr const T& R() const { return RGBA[0]; }
		constexpr const T& G() const { return RGBA[1]; }
		constexpr const T& B() const { return RGBA[2]; }
		constexpr const T& A() const { return RGBA[3]; }

		template<typename I>
		constexpr I Red() const
		{
			return R() * std::numeric_limits<I>::max();
		}

		template<typename I>
		constexpr I Green() const
		{
			return G() * std::numeric_limits<I>::max();
		}

		template<typename I>
		constexpr I Blue() const
		{
			return B() * std::numeric_limits<I>::max();
		}

		template<typename I>
		constexpr I Alpha() const
		{
			return A() * std::numeric_limits<I>::max();
		}

		
		constexpr static LinearColor ParseHexColor(const char* str, std::size_t len)
		{
			return (len == 9 && str[0] == '#')
				? LinearColor{
				HexPairToReal(str[1], str[2]),
				HexPairToReal(str[3], str[4]),
				HexPairToReal(str[5], str[6]),
				HexPairToReal(str[7], str[8])
			}
			: throw std::invalid_argument("Hex color must be in the format #rrggbbaa");
		}

	private:
		constexpr static int HexCharToInt(char c)
		{
			return ('0' <= c && c <= '9') ? c - '0' :
				('a' <= c && c <= 'f') ? c - 'a' + 10 :
				('A' <= c && c <= 'F') ? c - 'A' + 10 :
				throw std::invalid_argument("Invalid hex digit");
		}

		constexpr static T HexPairToReal(char hi, char lo)
		{
			int value = (HexCharToInt(hi) << 4) + HexCharToInt(lo);
			return static_cast<T>(value) / 255.0f;
		}

	};

	using LinearColorF = LinearColor<float>;

	constexpr LinearColorF operator""_frgba(const char* str, std::size_t len)
	{
		return LinearColorF::ParseHexColor(str, len);
	}
}
