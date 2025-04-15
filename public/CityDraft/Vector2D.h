#pragma once

#include <boost/geometry.hpp>
#include <boost/math/ccmath/abs.hpp>

namespace CityDraft
{
	/// <summary>
	/// Represents a real Vector in 2D space
	/// </summary>
	struct Vector2D
	{
	public:
		using UnderlyingType = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;

		UnderlyingType Data;

		constexpr Vector2D(double X, double Y) : Data{X, Y}
		{

		}

		constexpr Vector2D(const UnderlyingType& point) : Data{ point }
		{

		}

		constexpr operator UnderlyingType() const
		{
			return Data;
		}

		constexpr double GetX() const
		{
			return boost::geometry::get<0>(Data);
		}

		constexpr double GetY() const
		{
			return boost::geometry::get<1>(Data);
		}

		void SetX(double value)
		{
			boost::geometry::set<0>(Data, value);
		}

		void SetY(double value)
		{
			boost::geometry::set<1>(Data, value);
		}

		constexpr Vector2D operator+(const Vector2D& b) const
		{
			double x = GetX() + b.GetX();
			double y = GetY() + b.GetY();
			return Vector2D{ x, y };
		}

		Vector2D& operator+=(const Vector2D& b)
		{
			double x = GetX() + b.GetX();
			double y = GetY() + b.GetY();
			SetX(x);
			SetY(y);
			return *this;
		}

		constexpr Vector2D operator-(const Vector2D& b) const
		{
			double x = GetX() - b.GetX();
			double y = GetY() - b.GetY();
			return Vector2D{ x, y };
		}

		Vector2D& operator-=(const Vector2D& b)
		{
			double x = GetX() - b.GetX();
			double y = GetY() - b.GetY();
			SetX(x);
			SetY(y);
			return *this;
		}

		constexpr Vector2D operator*(double scalar) const
		{
			double x = GetX() * scalar;
			double y = GetY() * scalar;
			return Vector2D{ x, y };
		}

		Vector2D& operator*=(double scalar)
		{
			double x = GetX() * scalar;
			double y = GetY() * scalar;
			SetX(x);
			SetY(y);
			return *this;
		}

		constexpr Vector2D operator/(double scalar) const
		{
			double x = GetX() / scalar;
			double y = GetY() / scalar;
			return Vector2D{ x, y };
		}

		constexpr static bool IsNearlyZero(const Vector2D& a, double tolerance = 1E-9)
		{
			return boost::math::ccmath::abs(a.GetX()) < tolerance && boost::math::ccmath::abs(a.GetY()) < tolerance;
		}

		constexpr bool operator ==(const Vector2D& b)
		{
			return IsNearlyZero(*this - b);
		}

		constexpr bool operator !=(const Vector2D& b)
		{
			return !IsNearlyZero(*this - b);
		}
	};
}
