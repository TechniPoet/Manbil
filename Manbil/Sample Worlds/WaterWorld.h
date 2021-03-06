#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/Rendering.hpp"

#include "../Input/MovingCamera.h"
#include "../Rendering/Rendering.hpp"
#include "../Rendering/Water/Water.h"


//A sample world that demonstrates the Water system,
//    which is an example of how to extend the "DataNode" system.
//The Water class has two types of ripples:
//   "Directional" (waves along a direction on the surface)
//   "Circular" (ripples moving outward from a point)
//In this world, the directional ripples are set when the water is created,
//    and circular ripples are created by the player whenever he left-clicks.
class WaterWorld : public SFMLOpenGLWorld
{
public:
    WaterWorld(void);
    
protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override;
    virtual std::string GetWindowTitle(void) override;
    virtual sf::Uint32 GetSFStyleFlags(void) override;
    virtual sf::ContextSettings GenerateContext(void) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

private:

    Vector2u windowSize;
    MovingCamera gameCam;

    //Used for generating randomized ripples.
    FastRand fastRand;


    //The "Water" class contains its own mesh and parameters,
    //    so we don't need to manage that ourselves.
    std::unique_ptr<Water> waterObj;
	Transform waterTr;
	Water::RippleID mostRecentRipple = Water::RIPPLEID_INVALID;

	//The "Water" class does not contain its own material;
	//    we create one with the help of the "WaterNode" class.
    std::unique_ptr<Material> waterMat;
    MTexture2D normalMap1, normalMap2;


    //Display a skybox so that the water has something to reflect.
    std::unique_ptr<Material> skyboxMat;
    std::unique_ptr<Mesh> skyboxMesh;
    UniformDictionary skyboxParams;
    MTextureCubemap skyboxTex;

    
    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);
};