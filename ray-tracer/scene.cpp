#include "scene.h"

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <iostream>

void Scene::SetCameraFoV(const float fov) {
	const float PI = 3.14159265358979323846f;
	const float radian = fov * (PI / 180.0f);

	camera_.viewplane_distance = camera_.resolution.width / std::tan(radian / 2);
};

const std::vector<Math::Color>& Scene::Render() {
	// SimpleMath uses right-handed coordinate system
	// Use DirectXMath to make left-handed coordinates
	const Math::Matrix world_matrix = DirectX::XMMatrixLookAtLH(camera_.pos, camera_.forward, camera_.up);

	#pragma omp parallel for collapse(3) 
	for (int i = 0; i < camera_.resolution.height; i++) {
		for (int j = 0; j < camera_.resolution.width; j++) {
			Math::Vector3 viewplane_pixel = Math::Vector3(j + 0.5f - camera_.resolution.width / 2.0f, i + 0.5f - camera_.resolution.height / 2.0f, camera_.viewplane_distance);
			Math::Vector3 world_pixel = Math::Vector3::Transform(viewplane_pixel, world_matrix);
			Math::Vector3 ray_dir = world_pixel - camera_.pos;
			ray_dir.Normalize();

			Ray ray(camera_.pos, ray_dir);

			// find nearest sphere
			float t = -1.0f;
			const Sphere* nearest_sphere = nullptr;

			for (const Sphere& sphere : spheres_) {
				const float temp_t = sphere.FindIntersection(ray);
				if (temp_t < 0.0f) {
					continue;
				}

				if (t < 0.0f || temp_t < t) {
					t = temp_t;
					nearest_sphere = &sphere;
				}
			}

			// if ray does not intersect with any sphere
			if (t < 0.0f) {
				frame_buffer_[i * camera_.resolution.width + j] = Math::Color(0.0f, 0.0f, 0.0f, 1.0f);
				continue;
			}

			Math::Vector3 point = camera_.pos + ray_dir * t;
			frame_buffer_[i * camera_.resolution.width + j] = nearest_sphere->GetColor();
		}
	}

	return frame_buffer_;
}