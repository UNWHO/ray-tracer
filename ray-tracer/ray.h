#pragma once
#include <directxtk/SimpleMath.h>

namespace Math = DirectX::SimpleMath;

class Ray
{
private:
	Math::Vector3 pos_;
	Math::Vector3 dir_;

public:
	Ray(const Math::Vector3& pos, const Math::Vector3& dir): pos_(pos), dir_(dir) {}

	const Math::Vector3& GetPosition() const { return pos_; };
	const Math::Vector3& GetDirection() const { return dir_; };
};

