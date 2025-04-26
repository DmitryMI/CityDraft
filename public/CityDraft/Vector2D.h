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

		constexpr Vector2D() : Data{ 0, 0 }
		{

		}

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

		constexpr bool IsNearlyZero(double tolerance = 1E-9)
		{
			return boost::math::ccmath::abs(GetX()) < tolerance && boost::math::ccmath::abs(GetY()) < tolerance;
		}

		constexpr bool operator ==(const Vector2D& b)
		{
			return (*this - b).IsNearlyZero();
		}

		constexpr bool operator !=(const Vector2D& b)
		{
			return !(*this - b).IsNearlyZero();
		}

		constexpr double GetSizeSquared() const
		{
			return GetX() * GetX() + GetY() * GetY();
		}

		inline double GetSize() const
		{
			return sqrt(GetSizeSquared());
		}

		constexpr static double Dot(const Vector2D& a, const Vector2D& b)
		{
			return a.GetX() * b.GetX() + a.GetY() * b.GetY();
		}

		constexpr static double Cross(const Vector2D& a, const Vector2D& b)
		{
			return a.GetX() * b.GetY() - a.GetY() * b.GetX();
		}

		static inline double GetAngleBetweenPoints(const Vector2D& a, const Vector2D& b)
		{
			double dot = Dot(a, b);
			double cross = Cross(a, b);
			double angle = std::atan2(cross, dot);
			return angle;
		}

		Vector2D GetRotated(double angleRadians) const
		{
			Vector2D copy{ GetX(), GetY() };
			copy.Rotate(angleRadians);
			return copy;
		}

		Vector2D& Rotate(double angleRadians)
		{
			double cosA = std::cos(angleRadians);
			double sinA = std::sin(angleRadians);
			double x = GetX() * cosA - GetY() * sinA;
			double y = GetX() * sinA + GetY() * cosA;
			SetX(x);
			SetY(y);
			return *this;
		}

		constexpr static Vector2D Lerp(const Vector2D& a, const Vector2D& b, double t)
		{
			double x = (1 - t) * a.GetX() + t * b.GetX();
			double y = (1 - t) * a.GetY() + t * b.GetY();
			return {x, y};
		}

		Vector2D& ComponentMultiply(const Vector2D& other)
		{
			double x = GetX() * other.GetX();
			double y = GetY() * other.GetY();
			SetX(x);
			SetY(y);
			return *this;
		}
	};
}
