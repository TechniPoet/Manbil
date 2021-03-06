#include "TerrainWorld.h"

#include "../Math/NoiseGeneration.hpp"
#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Math/Higher Math/Terrain.h"

#include "../Input/Input.hpp"

#include "../DebugAssist.h"

#include <iostream>


typedef TerrainWorld TW;

//Use the Input system to track when a key was just pressed.
const unsigned int INPUT_NextLOD = 1,
                   INPUT_PrevLOD = 2;


TW::TerrainWorld(void)
    : windowSize(800, 600),
      terrainTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP),
                 PixelSizes::PS_32F, true),
      cam(Vector3f(0.0f, 0.0f, 50.0f), 40.0f, 0.16f, Vector3f(1.0f, 1.0f, 0.0f).Normalized()),
      SFMLOpenGLWorld(800, 600)
{

}
TW::~TerrainWorld(void)
{

}

sf::ContextSettings TerrainWorld::GenerateContext(void)
{
    return sf::ContextSettings(24, 0, 0, 4, 1);
}

void TW::GenerateTerrainLOD(const Terrain& terr, unsigned int lodLevel)
{
    std::vector<VertexPosUVNormal> verts;
    std::vector<unsigned int> inds;
    terr.GenerateTrianglesFull<VertexPosUVNormal>(verts, inds,
                                                  [](VertexPosUVNormal& v) { return &v.Pos; },
                                                  [](VertexPosUVNormal& v) { return &v.UV; },
                                                  [](VertexPosUVNormal& v) { return &v.Normal; },
                                                  100.0f, lodLevel);
    
    //Insert a mesh for the terrain with this level of detail.
	Mesh mesh(false, PrimitiveTypes::PT_TRIANGLE_LIST);
    mesh.SetVertexData(verts, Mesh::BUF_STATIC, VertexPosUVNormal::GetVertexAttributes());
    mesh.SetIndexData(inds, Mesh::BUF_STATIC);
	terrainMeshes.push_back(std::move(mesh));
}

void TW::InitializeTextures(void)
{
    terrainTex.Create();

    //Try to load it from a file.
    std::string errorMsg;
    if (!Assert(terrainTex.SetDataFromFile("Content/Sample Worlds/grass.png", errorMsg),
                "Error loading 'Content/Sample Worlds/grass.png'",
                errorMsg))
    {
        return;
    }
}
void TW::InitializeMaterials(void)
{
    SerializedMaterial matData(VertexPosUVNormal::GetVertexAttributes());

    std::vector<ShaderOutput> &vertOuts = matData.MaterialOuts.VertexOutputs,
                              &fragOuts = matData.MaterialOuts.FragmentOutputs;

    //Vertex shader is a simple object-to-screen-space conversion.
    //It outputs world position, UV, and world normal to the fragment shader.

    DataLine vIn_ObjPos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1),
             vIn_ObjNormal(VertexInputNode::GetInstance(), 2);

    DataNode::Ptr objPosToWorld = SpaceConverterNode::ObjPosToWorldPos(vIn_ObjPos);
    DataLine vOut_WorldPos(objPosToWorld);
    DataNode::Ptr vOut_WorldNormal(new SpaceConverterNode(vIn_ObjNormal,
                                                          SpaceConverterNode::ST_OBJECT,
                                                          SpaceConverterNode::ST_WORLD,
                                                          SpaceConverterNode::DT_NORMAL));
    vertOuts.push_back(ShaderOutput("vOut_WorldPos", vOut_WorldPos));
    vertOuts.push_back(ShaderOutput("vOut_UV", vIn_UV));
    vertOuts.push_back(ShaderOutput("vOut_WorldNormal", vOut_WorldNormal));

    DataNode::Ptr objPosToScreen = SpaceConverterNode::ObjPosToScreenPos(vIn_ObjPos);
    matData.MaterialOuts.VertexPosOutput = DataLine(objPosToScreen, 1);


    //Fragment shader combines the grass texture with ambient + diffuse lighting.

    DataLine fIn_WorldPos(FragmentInputNode::GetInstance(), 0),
             fIn_UV(FragmentInputNode::GetInstance(), 1),
             fIn_WorldNormal(FragmentInputNode::GetInstance(), 2);

    DataNode::Ptr normalizedNormal(new NormalizeNode(fIn_WorldNormal));

    DataLine lightDir(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
             ambientLight(0.8f),
             diffuseLight(0.5f),
             specLight(0.0f),
             specIntensity(256.0f);

    DataLine texScaleDown(513.0f);
    
    DataNode::Ptr finalUV(new MultiplyNode(fIn_UV, texScaleDown));
    DataNode::Ptr texSamplePtr(new TextureSample2DNode(finalUV, "u_tex"));
    DataLine texSample(texSamplePtr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));

    DataNode::Ptr lightCalc(new LightingNode(fIn_WorldPos, normalizedNormal, lightDir,
                                             ambientLight, diffuseLight,
                                             specLight, specIntensity));
    DataLine lightBrightness(lightCalc);

    DataNode::Ptr finalColor3(new MultiplyNode(lightBrightness, texSample)),
                  finalColor4(new CombineVectorNode(finalColor3, 1.0f));
    fragOuts.push_back(ShaderOutput("fOut_FinalColor4", finalColor4));


    //Generate the final material.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(matData, terrainParams,
                                                                                BlendMode::GetOpaque());
    if (Assert(genM.ErrorMessage.empty(), "Error generating terrain shaders", genM.ErrorMessage))
    {
        terrainMat.reset(genM.Mat);
    }


    //Set up parameters.
    terrainParams["u_tex"].Tex() = terrainTex.GetTextureHandle();
}
void TW::InitializeObjects(void)
{
    //Generate the terrain heightmap using layers of perlin noise.

    //Create Perlin noise at different scales and sum them together with different weights.
    const unsigned int nLevels = 8;
    Perlin2D pers[] =
    {
        Perlin2D(128.0f, Perlin2D::Quintic, Vector2i(), 166234, true),
        Perlin2D(64.0f, Perlin2D::Quintic, Vector2i(), 6543, true),
        Perlin2D(32.0f, Perlin2D::Quintic, Vector2i(), 666778, true),
        Perlin2D(16.0f, Perlin2D::Quintic, Vector2i(), 44, true),
        Perlin2D(8.0f, Perlin2D::Quintic, Vector2i(), 3356, true),
        Perlin2D(4.0f, Perlin2D::Quintic, Vector2i(), 5765, true),
        Perlin2D(2.0f, Perlin2D::Quintic, Vector2i(), 2353732, true),
        Perlin2D(1.0f, Perlin2D::Quintic, Vector2i(), 34532, true),
    };
    Generator2D* const generatorPointers[] = 
    {
        &pers[0],
        &pers[1],
        &pers[2],
        &pers[3],
        &pers[4],
        &pers[5],
        &pers[6],
        &pers[7],
    };

    std::vector<float> weights;
    for (unsigned int i = 0; i < nLevels; ++i)
    {
        weights.insert(weights.end(), 1.0f / powf(2.0f, (float)(i + 1)));
    }

    LayeredOctave2D layeredPerlin(nLevels, weights.data(), generatorPointers);

    Noise2D outNoise(513, 513);
    layeredPerlin.Generate(outNoise);


    //Generate the terrain vertices using the heightmap.

    Terrain terr(outNoise.GetDimensions());
    terr.SetHeightmap(outNoise);
    
    //Use six levels of detail.
    GenerateTerrainLOD(terr, 0);
    GenerateTerrainLOD(terr, 1);
    GenerateTerrainLOD(terr, 2);
    GenerateTerrainLOD(terr, 3);
    GenerateTerrainLOD(terr, 4);
    GenerateTerrainLOD(terr, 5);
    GenerateTerrainLOD(terr, 6);
}
void TW::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    if (IsGameOver())
    {
        return;
    }

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();
    
    //Set up the inputs.
    Input.AddBoolInput(INPUT_NextLOD,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Right,
                                                                      BoolInput::JustPressed)));
    Input.AddBoolInput(INPUT_PrevLOD,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Left,
                                                                      BoolInput::JustPressed)));


    //Set up the camera.
    cam.Window = GetWindow();
    cam.PerspectiveInfo.SetFOVDegrees(55.0f);
    cam.PerspectiveInfo.zFar = 4000.0f;
}

void TW::OnWorldEnd(void)
{
	terrainMat.reset();
	terrainMeshes.clear();
	terrainTex.DeleteIfValid();
}

void TW::UpdateWorld(float elapsedSeconds)
{
    cam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        EndWorld();
        return;
    }

    //Update input.
    if (Input.GetBoolInputValue(INPUT_NextLOD) &&
        currentTerrainMesh < terrainMeshes.size() - 1)
    {
		currentTerrainMesh += 1;
    }
    if (Input.GetBoolInputValue(INPUT_PrevLOD) &&
		currentTerrainMesh > 0)
    {
		currentTerrainMesh -= 1;
    }
}

void TW::RenderWorldGeometry(const RenderInfo& camInfo)
{
    //Render the current terrain mesh.
    terrainMat->Render(terrainMeshes[currentTerrainMesh], terrainTr, camInfo, terrainParams);
}
void TW::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    ScreenClearer().ClearScreen();
    RenderingState(RenderingState::C_BACK).EnableState();
    Viewport(0, 0, windowSize.x, windowSize.y).Use();

    //Calculate transforms.
    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetPerspectiveProjection(projM);

    //Render the world.
    RenderInfo info(GetTotalElapsedSeconds(), &cam, &viewM, &projM);
    RenderWorldGeometry(info);
}

void TW::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
    char dummy;
    std::cin >> dummy;
}

void TW::OnWindowResized(unsigned int newW, unsigned int newH)
{
    cam.PerspectiveInfo.Width = newW;
    cam.PerspectiveInfo.Height = newH;

    windowSize.x = newW;
    windowSize.y = newH;
}

bool TW::Assert(bool test, std::string errorIntro, const std::string& error)
{
    if (!test)
    {
        std::cout << errorIntro << ": " << error << "\nEnter anything to continue: ";
        char dummy;
        std::cin >> dummy;

        EndWorld();
    }

    return test;
}