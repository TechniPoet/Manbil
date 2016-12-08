#pragma once

#include "Game Loop\SFMLOpenGLWorld.h"
#include "Rendering\Rendering.hpp"
#include "Math\Shapes.hpp"
#include "Input\MovingCamera.h"

class TestWorld : public SFMLOpenGLWorld 
{
public:

	TestWorld();
	virtual ~TestWorld();

protected:

	virtual void InitializeWorld() override;
	virtual void OnWorldEnd() override;

	virtual void UpdateWorld(float deltaT) override;
	virtual void RenderOpenGL(float deltaT) override;

	virtual void OnInitializeError(std::string errorMsg) override;

private:

	MovingCamera cam;

	//A mesh to be drawn.
	std::unique_ptr<Mesh> mesh;

	std::unique_ptr<Material> material;
	UniformDictionary materialParams;

	std::unique_ptr<Box2D> collider;
	const Vector2f box1Pos;
	const Vector2f box1Scale;
	//The position, rotation, and scale of the mesh.
	Transform objTr;
};