#include "TestWorld.h"
#include "Input\Input.hpp"
#include <iostream>
#include "Rendering\Primitives\PrimitiveGenerator.h"


const Vector2u START_SIZE(800, 600);

const Vector3f CAM_START_POS(0.0f, 0.0f, 0.0f),
				CAM_START_FORWARD(0.0f, 1.0f, 0.0f),
				CAM_UPWARD(0.0f, 0.0f, 1.0f);

const float CAM_MOVE_SPEED = 0,
			CAM_TURN_SPEED = 0;

const float CAM_FOV_DEGREES = 60.0f,
			CAM_ZNEAR = 0.01f,
			CAM_ZFAR = 100.0f;

const Vector3f SKY_COLOR(0.65f, 0.75f, 1.0f);


const unsigned int INPUT_KEY_BRIGHTER = 123,
				   INPUT_KEY_DARKER = 321;

const float BRIGHTNESS_PER_SEC = .5f;
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


	auto* inputDarker = new MouseBoolInput(sf::Mouse::Button::Left,
		BoolInput::ValueStates::IsDown);
	auto* inputBrighter = new MouseBoolInput(sf::Mouse::Button::Right,
		BoolInput::ValueStates::IsDown);
	Input.AddBoolInput(INPUT_KEY_DARKER, BoolInputPtr(inputDarker));
	Input.AddBoolInput(INPUT_KEY_BRIGHTER, BoolInputPtr(inputBrighter));
	/*
	struct MyVertex {
		Vector3f Pos;
		MyVertex(const Vector3f& pos) : Pos(pos) { }
	};
	std::vector<MyVertex> vertices = {
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
	*/
	RenderIOAttributes vertexAttributes = VertexPosUVNormal::GetVertexAttributes();
	std::vector<VertexPosUVNormal> vertices;
	std::vector<unsigned int> indices;

	PrimitiveGenerator::GenerateCube(vertices, indices, false, false);
	b1Mesh.reset(new Mesh(false, PrimitiveTypes::PT_TRIANGLE_LIST));
	b1Mesh->SetVertexData(vertices, Mesh::BUF_STATIC, vertexAttributes);
	b1Mesh->SetIndexData(indices, Mesh::BUF_STATIC);
	b1Transform.SetScale(1);
	b1Transform.SetPosition(Vector3f(0.0f, 3.0f, 0.0f));
	b1Collider.reset(new Box2D(Vector2f(b1Transform.GetPosition().x, b1Transform.GetPosition().z),
		Vector2f(b1Transform.GetScale().x, b1Transform.GetScale().z)));
	MaterialUsageFlags builtInParams;
	builtInParams.EnableFlag(MaterialUsageFlags::DNF_USES_WVP_MAT);

	std::string vShader =
		MaterialConstants::GetVertexHeader(
			"out vec3 vOut_Color;",
			vertexAttributes,
			builtInParams);
	vShader += std::string() + R"(
uniform float u_brightness;
void main()
{
	gl_Position = )" + MaterialConstants::WVPMatName + R"( * vec4(vIn_Pos, 1.0);
	vOut_Color = vIn_Pos * u_brightness;
}
)";
	builtInParams.ClearAllFlags();
	std::string fShader =
		MaterialConstants::GetFragmentHeader(
			"in vec3 vOut_Color;",
			"out vec4 fOut_PixelColor;",
			builtInParams);
	fShader += std::string() + R"(
void main()
{
	fOut_PixelColor = vec4(vOut_Color, 1.0);
}
)";

	std::string errMsg;
	float initialBrightness = 1.0f;
	b1MatParams["u_brightness"] =
		Uniform::MakeF("u_brightness", 1, &initialBrightness);
	b1Mat.reset(new Material(vShader, fShader, b1MatParams, vertexAttributes,
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
	b1Mesh.reset();
	b1Mat.reset();
}

void TestWorld::UpdateWorld(float deltaT)
{
	cam.Update(deltaT);

	float currentBrightness = b1MatParams["u_brightness"].Float().GetValue()[0];
	if (Input.GetBoolInputValue(INPUT_KEY_DARKER)) {
		currentBrightness -= BRIGHTNESS_PER_SEC * deltaT;
	}
	if (Input.GetBoolInputValue(INPUT_KEY_BRIGHTER)) {
		currentBrightness += BRIGHTNESS_PER_SEC * deltaT;
	}
	currentBrightness = Mathf::Clamp(currentBrightness, 0.0f, 5.0f);

	b1MatParams["u_brightness"].Float() = currentBrightness;
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

	b1Mat->Render(*b1Mesh,  b1Transform, cameraData, b1MatParams);
}

void TestWorld::OnInitializeError(std::string errorMsg)
{
	SFMLOpenGLWorld::OnInitializeError(errorMsg);
	EndWorld();
}