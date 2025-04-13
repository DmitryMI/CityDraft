#pragma once

#include <Eigen/Geometry>

namespace CityDraft
{
	/// <summary>
	/// Represents a transformation in 2D space
	/// </summary>
	using Transform2D = Eigen::Transform<double, 2, Eigen::Affine>;
}
