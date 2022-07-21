#include "angled_aero.h"

AngledAero::AngledAero(const DirectX::XMFLOAT3X3& tensor, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3* windspeed) : Aero(tensor, position, windspeed) {}

AngledAero::AngledAero(DirectX::FXMMATRIX tensor, DirectX::FXMVECTOR position, const DirectX::XMFLOAT3* windspeed) : Aero(tensor, position, windspeed) {}

void AngledAero::setOrientation(const DirectX::XMFLOAT4& quat) {}

void AngledAero::setOrientation(DirectX::XMVECTOR quat) {}

void AngledAero::updateForce(RigidBody* body, float duration) {}
