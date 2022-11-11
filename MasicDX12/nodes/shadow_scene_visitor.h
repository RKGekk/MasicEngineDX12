#pragma once

#include <memory>
#include <vector>

#include <DirectXMath.h>

#include "i_visitor.h"

class CameraNode;
class EffectShadowPSO;
class CommandList;

class ShadowSceneVisitor : public IVisitor {
public:
    ShadowSceneVisitor(CommandList& command_list, std::shared_ptr<CameraNode> camera, EffectShadowPSO& pso, bool transparent);

    void ResetCamera();
    void ResetBoneTransforms(const std::vector<DirectX::XMFLOAT4X4>& final_transforms);

    virtual void Visit(std::shared_ptr<SceneNode> scene_node) override;

private:
    CommandList& m_command_list;
    std::shared_ptr<CameraNode> m_camera;
    EffectShadowPSO& m_shadow_pso;
    bool m_transparent_pass;
};