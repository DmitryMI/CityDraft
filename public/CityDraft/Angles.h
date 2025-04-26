#pragma once

#include <numbers>

namespace CityDraft
{
	struct Degrees;

	struct Radians
	{
		double Value;
		constexpr explicit Radians(double val): Value(val)
		{
		}

		constexpr operator Degrees() const;

		constexpr Radians operator+(const Radians& other) const
		{
			double value = Value + other.Value;
			return Radians(value);
		}

		constexpr Radians operator-(const Radians& other) const
		{
			double value = Value - other.Value;
			return Radians(value);
		}

		constexpr Radians operator*(const Radians& other) const
		{
			double value = Value * other.Value;
			return Radians(value);
		}

		constexpr Radians operator/(const Radians& other) const
		{
			double value = Value / other.Value;
			return Radians(value);
		}

		Radians& operator +=(const Radians& other)
		{
			Value += other.Value;
			return *this;
		}

		Radians& operator -=(const Radians& other)
		{
			Value -= other.Value;
			return *this;
		}

		Radians& operator *=(const Radians& other)
		{
			Value *= other.Value;
			return *this;
		}

		Radians& operator /=(const Radians& other)
		{
			Value /= other.Value;
			return *this;
		}

		constexpr bool operator==(const Radians& other)
		{
			return Value == other.Value;
		}

		constexpr bool operator!=(const Radians& other)
		{
			return Value != other.Value;
		}

		constexpr bool operator<(const Radians& other)
		{
			return Value < other.Value;
		}

		constexpr bool operator>(const Radians& other)
		{
			return Value > other.Value;
		}
	};

	

	struct Degrees
	{
		double Value;
		constexpr explicit Degrees(double val): Value(val)
		{}

		constexpr operator Radians() const;

		constexpr Degrees operator+(const Degrees& other) const
		{
			double value = Value + other.Value;
			return Degrees(value);
		}

		constexpr Degrees operator-(const Degrees& other) const
		{
			double value = Value - other.Value;
			return Degrees(value);
		}

		constexpr Degrees operator*(const Degrees& other) const
		{
			double value = Value * other.Value;
			return Degrees(value);
		}

		constexpr Degrees operator/(const Degrees& other) const
		{
			double value = Value / other.Value;
			return Degrees(value);
		}

		Degrees& operator +=(const Degrees& other)
		{
			Value += other.Value;
			return *this;
		}

		Degrees& operator -=(const Degrees& other)
		{
			Value -= other.Value;
			return *this;
		}

		Degrees& operator *=(const Degrees& other)
		{
			Value *= other.Value;
			return *this;
		}

		Degrees& operator /=(const Degrees& other)
		{
			Value /= other.Value;
			return *this;
		}

		constexpr bool operator==(const Degrees& other)
		{
			return Value == other.Value;
		}

		constexpr bool operator!=(const Degrees& other)
		{
			return Value != other.Value;
		}

		constexpr bool operator<(const Degrees& other)
		{
			return Value < other.Value;
		}

		constexpr bool operator>(const Degrees& other)
		{
			return Value > other.Value;
		}
	};

	constexpr Radians operator""_rad(long double angle)
	{
		return Radians(static_cast<double>(angle));
	}

	constexpr Radians operator""_rad(unsigned long long angle)
	{
		return Radians(static_cast<double>(angle));
	}

	constexpr Degrees operator""_deg(long double angle)
	{
		return Degrees(static_cast<double>(angle));
	}

	constexpr Degrees operator""_deg(unsigned long long angle)
	{
		return Degrees(static_cast<double>(angle));
	}

	constexpr Radians::operator Degrees() const
	{
		return Degrees(Value * (180.0 / std::numbers::pi));
	}

	constexpr Degrees::operator Radians() const
	{
		return Radians(Value * (std::numbers::pi / 180.0));
	}

}