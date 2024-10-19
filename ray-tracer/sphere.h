#pragma once
#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <cmath>

#include "ray.h"

namespace Math = DirectX::SimpleMath;

struct Phong {
	float ambient;
	float diffuse;
	float specular;
	int specular_n;
};

class Sphere
{
private:
	Math::Vector3 pos_;
	Math::Color color_;
	float radius_;
	float radius_square_;

	Phong phong_;


public:
	Sphere(const Math::Vector3& pos, const Math::Color& color, const float radius) : pos_(pos), color_(color), radius_(radius), radius_square_(radius* radius) {};

	const Math::Vector3& GetPosition() const { return pos_; };
	const Math::Color& GetColor() const { return color_; };

	float* GetPositionData()  { return &pos_.x; };
	float* GetColorData()  { return &color_.x; };

	Phong* GetPhongParameterData() { return &phong_; };
	void SetPhongParameter(float ambient, float diffuse, float specular, int specular_n) {
		phong_.ambient = ambient;
		phong_.diffuse = diffuse;
		phong_.specular = specular;
		phong_.specular_n = specular_n;
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

	Math::Color CalculateColor (const Math::Vector3& point, const Math::Vector3& eye_pos, const Math::Vector3& light_pos, const Math::Color& light_color) const {
		Math::Vector3 N = point - pos_;
		N.Normalize();
		Math::Vector3 L = light_pos - point;
		L.Normalize();
		const float diffuse = phong_.diffuse * std::max<float>(N.Dot(L), 0.0f);

		Math::Vector3 V = eye_pos - point;
		V.Normalize();
		Math::Vector3 R = (2.0f * N.Dot(L)) * N - L;
		R.Normalize();
		const float specular = phong_.specular * std::powf(std::max<float>(V.Dot(R), 0.0f), static_cast<float>(phong_.specular_n));

		Math::Color color =
			phong_.ambient * color_
			+ diffuse * light_color
			+ specular * light_color;

		color.x = std::min<float>(color.x, 1.0f);
		color.y = std::min<float>(color.y, 1.0f);
		color.z = std::min<float>(color.z, 1.0f);
		color.w = 1.0f;

		return color;
	}

};

