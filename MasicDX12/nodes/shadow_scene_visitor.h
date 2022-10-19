#pragma once

#include <memory>

#include "i_visitor.h"

class CameraNode;
class EffectShadowPSO;
class CommandList;

class ShadowSceneVisitor : public IVisitor {
public:
    ShadowSceneVisitor(CommandList& command_list, std::shared_ptr<CameraNode> camera, EffectShadowPSO& pso, bool transparent);

    void ResetCamera();

    virtual void Visit(std::shared_ptr<SceneNode> scene_node) override;

private:
    CommandList& m_command_list;
    std::shared_ptr<CameraNode> m_camera;
    EffectShadowPSO& m_lighting_pso;
    bool m_transparent_pass;
};