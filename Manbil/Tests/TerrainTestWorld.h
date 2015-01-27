#pragma once

#include "../SFMLOpenGLWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../MovingCamera.h"


class Terrain;

//A simple world that tests the Terrain class.
class TerrainTestWorld : public SFMLOpenGLWorld
{
public:

    TerrainTestWorld(void);
    virtual ~TerrainTestWorld(void);


protected:

    virtual void InitializeWorld(void) override;
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newW, unsigned int newH) override;


private:

    //If the given test value is false, the following is done:
    //  1) Prints the given error, then pauses for the user to acknowledge.
    //  2) Ends the world.
    //  3) Returns false.
    //Otherwise, returns true.
    bool Assert(bool test, std::string errorIntro, const std::string& error);


    void GenerateTerrainLOD(const Terrain& terr, unsigned int lodLevel);

    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);

    //Renders the world geometry using the given rendering info.
    //Returns whether there was an error rendering.
    bool RenderWorldGeometry(const RenderInfo& info);


    Material* terrMat;
    Mesh terrMesh;
    UniformDictionary terrParams;
    MTexture2D terrTex;

    Vector2u windowSize;

    MovingCamera cam;
};