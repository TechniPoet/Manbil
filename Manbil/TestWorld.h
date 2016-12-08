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
	std::unique_ptr<Mesh> b1Mesh;

	std::unique_ptr<Material> b1Mat;
	UniformDictionary b1MatParams;

	std::unique_ptr<Box2D> b1Collider;

	//The position, rotation, and scale of the mesh.
	Transform b1Transform;
};