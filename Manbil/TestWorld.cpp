#include "TestWorld.h"

const Vector2u START_SIZE(800, 600);

const Vector3f CAM_START_POS(0.0f, 0.0f, 0.0f),
				CAM_START_FORWARD(0.0f, 1.0f, 0.0f),
				CAM_UPWARD(0.0f, 0.0f, 1.0f);

const float CAM_MOVE_SPEED = 10.0f,
			CAM_TURN_SPEED = 1.5f;

const float CAM_FOV_DEGREES = 60.0f,
			CAM_ZNEAR = 0.01f,
			CAM_ZFAR = 100.0f;

const Vector3f SKY_COLOR(0.65f, 0.75f, 1.0f);

// Constructor
TestWorld::TestWorld() 
	: SFMLOpenGLWorld(START_SIZE.x, START_SIZE.y)
{

}

// Destructor
TestWorld::~TestWorld()
{

}

void TestWorld::InitializeWorld()
{
	SFMLOpenGLWorld::InitializeWorld();

	if (IsGameOver())
	{
		return;
	}

	cam = MovingCamera(CAM_START_POS, CAM_MOVE_SPEED, CAM_TURN_SPEED,
		CAM_START_FORWARD, CAM_UPWARD, true);
	cam.Window = GetWindow();

	cam.PerspectiveInfo.SetFOVDegrees(CAM_FOV_DEGREES);
	cam.PerspectiveInfo.Width = GetWindow()->getSize().x;
	cam.PerspectiveInfo.Height = GetWindow()->getSize().y;
	cam.PerspectiveInfo.zNear = CAM_ZNEAR;
	cam.PerspectiveInfo.zFar = CAM_ZFAR;
}
void TestWorld::OnWorldEnd()
{

}

void TestWorld::UpdateWorld(float deltaT)
{
	cam.Update(deltaT);
}

void TestWorld::RenderOpenGL(float deltaT)
{
	Matrix4f viewMat, projMat;
	cam.GetViewTransform(viewMat);
	cam.GetPerspectiveProjection(projMat);

	RenderInfo cameraData(GetTotalElapsedSeconds(), &cam, &viewMat, &projMat);

	ScreenClearer clearer;
	clearer.ClearColor = true; 
	clearer.ClearedColor = Vector4f(SKY_COLOR, 1.0f);
	clearer.ClearDepth = true;
	clearer.ClearedDepth = 1.0f;
	clearer.ClearScreen();

	RenderingState renderState(RenderingState::C_BACK, true, true);
	renderState.EnableState();

	Viewport view(0, 0, GetWindow()->getSize().x, GetWindow()->getSize().y);
	view.Use();

}

void TestWorld::OnInitializeError(std::string errorMsg)
{
	SFMLOpenGLWorld::OnInitializeError(errorMsg);
	EndWorld();
}