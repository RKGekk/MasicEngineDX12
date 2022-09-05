#include "human_view.h"

HumanView::HumanView() {
	m_process_manager = std::make_unique<ProcessManager>();

	m_pointer_radius = 1.0f;
	m_view_id = 0xffffffff;

	RegisterAllDelegates();
	m_base_game_state = BaseEngineState::BGS_Initializing;

	
	m_scene.reset(new ScreenElementScene(renderer));

	Frustum frustum;
	//frustum.Init(DirectX::XM_PI / 4.0f, 1.0f, 1.0f, 100.0f);
	//frustum.Init(DirectX::XM_PI / 4.0f, 1.0f, 0.1f, 100.0f);
	const EngineOptions& options = g_pApp->GetConfig();
	frustum.Init(
		DirectX::XMConvertToRadians(options.m_fov),
		((float)options.m_screenWidth) / ((float)options.m_screenHeight),
		options.m_screenNear,
		options.m_screenFar
	);
	//frustum.Init(DirectX::XM_PI / 4.0f, 1280.0f/720.0f, 0.1f, 100.0f);
	//frustum.Init(DirectX::XM_PI / 2.0f, 1.0f, 0.1f, 100.0f);
	m_camera.reset(new CameraNode(DirectX::XMMatrixIdentity(), frustum));

	m_scene->VAddChild(INVALID_ACTOR_ID, INVALID_COMPONENT_ID, m_camera);
	m_scene->SetCamera(m_camera);
}
