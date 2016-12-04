#pragma once

#include "Game Loop\SFMLOpenGLWorld.h"
#include "Rendering\Rendering.hpp"

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
};