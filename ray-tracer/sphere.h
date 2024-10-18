#pragma once
#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <cmath>

#include "ray.h"

namespace Math = DirectX::SimpleMath;

class Sphere
{
private:
	Math::Vector3 pos_;
	Math::Color color_;
	float radius_;
	float radius_square_;

public:
	Sphere(const Math::Vector3& pos, const Math::Color& color, const float radius) : pos_(pos), color_(color), radius_(radius), radius_square_(radius* radius) {};

	const Math::Color& GetColor() const { return color_; };

	bool IsInside(const Math::Vector3 point) const {
		Math::Vector3 vec = point - pos_;
		return vec.Dot(vec) < radius_square_;
	}

	float FindIntersection(const Ray& ray) const {
		Math::Vector3 line = pos_ - ray.GetPosition();

		float a = ray.GetDirection().Dot(ray.GetDirection());
		float b = -2.0f * ray.GetDirection().Dot(line);
		float c = line.Dot(line) - radius_square_;

		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0.0f) {
			return -1.0f;
		}
		discriminant = std::sqrtf(discriminant);

		float t = std::min<float>(-b - discriminant, -b + discriminant) / (2 * a);
		return t;
	}

};

