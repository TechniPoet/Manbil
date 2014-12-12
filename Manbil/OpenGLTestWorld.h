#pragma once


#include "SFMLOpenGLWorld.h"
#include "Math/HigherMath.hpp"
#include "MovingCamera.h"
#include "Mesh.h"
#include "Math/Higher Math/Lighting.h"
#include "Rendering/Water/Water.h"
#include "Rendering/PostProcessing/PostProcessChain.h"
#include "Rendering/GPU Particles/High-level GPU Particles/HGPComponentManager.h"
#include "Rendering/GUI/TextRenderer.h"
#include "Rendering/Texture Management/MTextureCubemap.h"
#include "Rendering/Texture Management/MTexture3D.h"


class OpenGLTestWorld : public SFMLOpenGLWorld
{
public:

	OpenGLTestWorld(void);
	~OpenGLTestWorld(void);

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;

	virtual void RenderOpenGL(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void OnWorldEnd(void) override;

private:

    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);

	void RenderWorldGeometry(const RenderInfo & info);

    Water * water;
    Material * waterMat;

    Mesh gsMesh;
    Material * gsTestMat;
    MTexture3D gsTestTex3D;
    UniformDictionary gsTestParams;

    HGPComponentManager particleManager;
    Mesh particleMesh;
    Material * particleMat;
    UniformDictionary particleParams;

	MovingCamera cam;

    unsigned int worldRenderID;
    MTexture2D worldColorTex1, worldColorTex2, worldDepthTex;

    PostProcessChain * ppc;
    std::vector<std::shared_ptr<PostProcessEffect>> ppcChain;

    Mesh cubemapMesh;
    UniformDictionary cubemapParams;
    Material * cubemapMat;
    MTextureCubemap cubemapTex;

    DrawingQuad * finalScreenQuad;
    Material * finalScreenMat;
    UniformDictionary finalScreenQuadParams;

    TextRenderer::FontSlot testFontSlot;

    MTexture2D waterNormalTex1, waterNormalTex2;
};