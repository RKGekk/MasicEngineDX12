#pragma once

#include <filesystem>
#include <memory>

class CommandList;
class Material;
class Mesh;
class SceneNode;

class MeshNodeLoader {
public:
	MeshNodeLoader(const MeshNodeLoader&) = delete;
	MeshNodeLoader(MeshNodeLoader&&) = delete;
	MeshNodeLoader& operator=(const MeshNodeLoader&) = delete;
	MeshNodeLoader& operator=(MeshNodeLoader&&) = delete;

	static std::shared_ptr<SceneNode> ImportSceneNode(CommandList& command_list, const std::filesystem::path& file_name, bool is_inv_y_texture);

private:
	MeshNodeLoader() = default;
	~MeshNodeLoader() = default;
};