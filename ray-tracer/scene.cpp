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

	const float ambient_coef = 0.2f;
	const float diffuse_coef = 0.4f;
	const float specular_coef = 0.4f;
	const int specular_n = 10;

	//#pragma omp parallel for collapse(3) 
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
			
			Math::Vector3 N = point - nearest_sphere->GetPosition();
			N.Normalize();
			Math::Vector3 L = light_.pos - point;
			L.Normalize();
			const float diffuse = diffuse_coef * std::max<float>(N.Dot(L), 0.0f);
		
			Math::Vector3 V = -ray_dir;
			V.Normalize();
			Math::Vector3 R = (2.0f * N.Dot(L)) * N - L;
			R.Normalize();
			const float specular = specular_coef * std::powf(std::max<float>(V.Dot(R), 0.0f), specular_n);

			Math::Color color = 
				ambient_coef * nearest_sphere->GetColor()
				+ diffuse * light_.color
				+ specular * light_.color;

			color.x = std::min<float>(color.x, 1.0f);
			color.y = std::min<float>(color.y, 1.0f);
			color.z = std::min<float>(color.z, 1.0f);
			color.w = 1.0f;

			frame_buffer_[i * camera_.resolution.width + j] = color;
		}
	}

	return frame_buffer_;
}