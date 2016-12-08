#include "TestWorld.h"

#include <iostream>

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

	struct MyVertex {
		Vector3f Pos;
		MyVertex(const Vector3f& pos) : Pos(pos) { }
	};
	std::vector<MyVertex> verticies = {
		MyVertex(Vector3f(0, 0, 0)),
		MyVertex(Vector3f(0, 0, 1)),
		MyVertex(Vector3f(0, 1, 0)),
		MyVertex(Vector3f(0, 1, 1)),
		MyVertex(Vector3f(1, 0, 0)),
		MyVertex(Vector3f(1, 0, 1)),
		MyVertex(Vector3f(1, 1, 0)),
		MyVertex(Vector3f(1, 1, 1)),
	};
	std::vector<unsigned int> indices = {
		0, 2, 1,	3, 1, 2,
		0, 1, 4,	5, 4, 1,
		2, 0, 6,	4, 6, 0,
		2, 6, 3,	7, 3, 6,
		4, 5, 6,	5, 7, 6,
		1, 3, 5,	7, 5, 3,
	};

	RenderIOAttributes vertexAttributes(
		RenderIOAttributes::Attribute(3, false, "vIn_Pos"));

	mesh.reset(new Mesh(false, PrimitiveTypes::PT_TRIANGLE_LIST));
	mesh->SetVertexData(verticies, Mesh::BUF_STATIC, vertexAttributes);
	mesh->SetIndexData(indices, Mesh::BUF_STATIC);

	objTr.SetScale(1);
	objTr.SetPosition(Vector3f(0.0f, 0.5f, 0.0f));

	MaterialUsageFlags builtInParams;
	builtInParams.EnableFlag(MaterialUsageFlags::DNF_USES_WVP_MAT);

	std::string vShader =
		MaterialConstants::GetVertexHeader(
			"",
			vertexAttributes,
			builtInParams);
	vShader += std::string() + R"(
void main()
{
	gl_Position = )" + MaterialConstants::WVPMatName + R"( * vec4(vIn_Pos, 1.0);
}
)";
	builtInParams.ClearAllFlags();
	std::string fShader =
		MaterialConstants::GetFragmentHeader(
			"",
			"out vec4 fOut_PixelColor;",
			builtInParams);
	fShader += std::string() + R"(
void main()
{
	fOut_PixelColor = vec4(1.0, 0.25, 0.25, 1.0);
}
)";

	std::string errMsg;
	material.reset(new Material(vShader, fShader, materialParams, vertexAttributes,
		BlendMode::GetOpaque(), errMsg));

	if (errMsg != "") {
		std::cout << "Error compiling material: " << errMsg << "\n";
		char waitForKey;
		std::cin >> waitForKey;
		EndWorld();
	}
}
void TestWorld::OnWorldEnd()
{
	mesh.reset();
	material.reset();
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

	material->Render(*mesh,  objTr, cameraData, materialParams);
}

void TestWorld::OnInitializeError(std::string errorMsg)
{
	SFMLOpenGLWorld::OnInitializeError(errorMsg);
	EndWorld();
}