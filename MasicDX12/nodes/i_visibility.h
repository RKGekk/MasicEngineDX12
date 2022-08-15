#pragma once

#pragma once

#include <memory>

class MeshNode;
class CameraNode;

class IVisibility {
public:
	IVisibility() = default;
	virtual ~IVisibility() = default;

	virtual bool VIsVisible(std::shared_ptr<MeshNode> mesh, std::shared_ptr<CameraNode> cam) const = 0;
};