#pragma once

#include <memory>

#include "i_visitor.h"

class CameraNode;
class EffectPSO;
class CommandList;

class SceneVisitor : public IVisitor {
public:
    SceneVisitor(CommandList& command_list, std::shared_ptr<CameraNode> camera, EffectPSO& pso, bool transparent);

    void ResetCamera();

    virtual void Visit(std::shared_ptr<SceneNode> scene_node) override;

private:
    CommandList& m_command_list;
    std::shared_ptr<CameraNode> m_camera;
    EffectPSO& m_lighting_pso;
    bool m_transparent_pass;
};