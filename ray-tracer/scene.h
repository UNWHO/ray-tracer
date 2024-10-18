#pragma once
#include <vector>

#include "sphere.h"

struct Camera {
	Math::Vector3 pos;
	Math::Vector3 forward;
	Math::Vector3 up;
	float viewplane_distance;

	struct {
		unsigned int width;
		unsigned int height;
	} resolution;
};

class Scene
{
private:
	Camera camera_;
	std::vector<Sphere> spheres_;

	std::vector<Math::Color> frame_buffer_;

public:
	void SetCameraPosition(const Math::Vector3& pos) { camera_.pos = pos; };
	void SetCameraDirection(const Math::Vector3& forward, const Math::Vector3& up) {
		camera_.forward = forward;
		camera_.forward.Normalize();

		camera_.up = up;
		camera_.up.Normalize();
	};
	void SetCameraFoV(const float fov);
	void SetCameraResolution(const unsigned int width, const unsigned int height) {
		camera_.resolution.width = width;
		camera_.resolution.height = height;
		frame_buffer_.resize(width * height);
	}



	void AddSphere(const Sphere& sphere) {
		spheres_.push_back(sphere);
	}

	const std::vector<Math::Color>& Render();
};

