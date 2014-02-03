#include "OpenGLTestWorld.h"

#include "Material.h"
#include <iostream>
#include "Vertices.h"
#include "ScreenClearer.h"
#include "RenderingState.h"
#include "RenderDataHandler.h"
#include "TextureSettings.h"

#include "Math/NoiseGeneration.hpp"

#include <assert.h>


namespace OGLTestPrints
{
	bool PrintRenderError(const char * errorIntro)
{
	const char * error = GetCurrentRenderingError();
	if (strcmp(error, "") != 0)
	{
		std::cout << errorIntro << ": " << error << "\n";
		ClearAllRenderingErrors();
		return false;
	}

	return true;
}

	void Pause(void)
	{
		char dummy;
		std::cin >> dummy;
	}
}
using namespace OGLTestPrints;


const Vector2i windowSize(1000, 700);

const RenderingState worldRenderState;


sf::Image img;
RenderObjHandle imgObj;

const int terrainSize = 300;
const float terrainBreadth = 5.0f, terrainHeight = 100.0f;
const float terrainTexScale = 50.0f;
void GenerateTerrainNoise(Noise2D & outNoise)
{
	Generator * finalG = 0;

	//WhiteNoise whiteNoise;
	//Interpolator interp(&whiteNoise, terrainSize, terrainSize, 5.0f);
	//finalG = &interp;

	Perlin per(60.0f, Perlin::Smoothness::Quintic);
	Perlin per2(30.0f, Perlin::Smoothness::Cubic);
	Perlin per3(5.0f, Perlin::Smoothness::Linear);

	float weights[3] = { 0.75f, 0.25f, 0.0425f };
	Generator * gens[3] = { &per, &per2, &per3 };
	LayeredOctave lay(3, weights, gens);
	finalG = &lay;

    NoiseFilterer filterer;
    filterer.FilterFunc = &NoiseFilterer::Increase;
    filterer.Increase_Amount = 1.0f;
    CircularFilterRegion circleReg(Vector2f(), 500.0f, 1.0f, 0.2f);
    filterer.FillRegion = &circleReg;
    filterer.NoiseToFilter = &lay;
    finalG = &filterer;

	if (finalG != 0) finalG->Generate(outNoise);
}


bool ShouldUseFramebuffer(void) { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift); }


OpenGLTestWorld::OpenGLTestWorld(void)
: SFMLOpenGLWorld(windowSize.x, windowSize.y), testMat(0), testMesh(PrimitiveTypes::Triangles), foliage(0), pTerr(0)
{
	dirLight.Dir = Vector3f(1.0f, 1.0f, -1.0f).Normalized();
	dirLight.Col = Vector3f(1.0f, 1.0f, 1.0f);

	dirLight.Ambient = 0.05f;
	dirLight.Diffuse = 0.7f;
	dirLight.Specular = 4.0f;
	
	dirLight.SpecularIntensity = 128.0f;
}
void OpenGLTestWorld::InitializeWorld(void)
{
	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;
	

	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);


	if (!img.loadFromFile("Grass.png"))
	{
		std::cout << "Failed to load grass texture.\n";
		Pause();
		EndWorld();
		return;
	}
	RenderDataHandler::CreateTexture2D(imgObj, img, true);
	
	TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP).SetData(imgObj);
	if (!PrintRenderError("Error setting up grass texture"))
	{
		Pause();
		EndWorld();
		return;
	}

	sf::Image otherImg;
	RenderObjHandle otherImgH;
	if (!otherImg.loadFromFile("shrub.png"))
	{
		std::cout << "Failed to load shrub texture.\n";
		Pause();
		EndWorld();
		return;
	}
	RenderDataHandler::CreateTexture2D(otherImgH, otherImg);

	TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP).SetData(otherImgH);
	if (!PrintRenderError("Error setting up shrub texture"))
	{
		Pause();
		EndWorld();
		return;
	}


    //Material.
    std::vector<RenderingPass> mats;
    mats.insert(mats.end(), Materials::LitTexture);
    testMat = new Material(mats);
    if (testMat->HasError())
    {
        std::cout << "Lit Texture material error: " << testMat->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }
    if (!Materials::LitTexture_GetUniforms(*testMat))
    {
        std::cout << "Lit texture directional light uniform get location error.\n";
    }
    if (!Materials::LitTexture_SetUniforms(*testMat, dirLight))
    {
        std::cout << "Lit texture directional light uniform set error.\n";
    }
    testMat->SetUniformF("u_textureScale", &terrainTexScale, 1);

	//Create vertices.

	RenderObjHandle vs, is;
	VertexIndexData vid;

	Noise2D noise(terrainSize, terrainSize);
	GenerateTerrainNoise(noise);

    pTerr = new Terrain(terrainSize);
	Terrain & terr = *pTerr;
	terr.SetHeightmap(noise);

	const int size = terrainSize * terrainSize;
	assert(size == terr.GetVerticesCount());
	Vertex * vertices = new Vertex[size];
	Vector3f * vertexPoses = new Vector3f[size], * vertexNormals = new Vector3f[size];
	Vector2f * vertexTexCoords = new Vector2f[size];
	unsigned int * indices = new unsigned int[terr.GetIndicesCount()];

	terr.CreateVertexPositions(vertexPoses, Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));
	for (int i = 0; i < size; ++i)
	{
		vertexPoses[i].x *= terrainBreadth;
		vertexPoses[i].y *= terrainBreadth;
		vertexPoses[i].z *= terrainHeight;
	}
	terr.CreateVertexNormals(vertexNormals, vertexPoses, Vector3f(1.0f, 1.0f, 1.0f), Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));
	terr.CreateVertexTexCoords(vertexTexCoords, Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));
	terr.CreateVertexIndices(indices, Vector2i(0, 0), Vector2i(terrainSize - 1, terrainSize - 1));

	for (int i = 0; i < size; ++i)
	{
		vertices[i] = Vertex(vertexPoses[i], vertexTexCoords[i], vertexNormals[i]);
	}

	RenderDataHandler::CreateVertexBuffer(vs, vertices, size, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	RenderDataHandler::CreateIndexBuffer(is, indices, terr.GetIndicesCount(), RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	vid = VertexIndexData(size, vs, terr.GetIndicesCount(), is);


	if (!PrintRenderError("Error creating vertex/index buffer for terrain"))
	{
		Pause();
		EndWorld();
		return;
	}

	delete[] vertices, vertexNormals, vertexTexCoords, indices;

	//Create the foliage before deleting the vertex data.
	std::vector<Vector3f> poses;
    FastRand fr;
	for (int i = terrainSize; i < size; i += 1 + (BasicMath::Abs(fr.GetRandInt()) % 200))
	{
		poses.insert(poses.end(), vertexPoses[i]);
	}
	foliage = new Foliage(poses, Vector2f(10.0f, 4.0f));
	if (foliage->HasError())
	{
		std::cout << "Error creating foliage: " << foliage->GetError();
		Pause();
		EndWorld();
		return;
	}
    foliage->SetTexture(otherImgH);
    foliage->SetWaveSpeed(0.5f);
    foliage->SetWaveScale(2.0f);
    foliage->SetLeanAwayMaxDist(10.0f);
    foliage->SetBrightness(0.5f);
    //Materials::LitTexture_SetUniforms(*foliage->Mat, dirLight);
	
	delete[] vertexPoses;


    //Camera.
    Vector3f pos(0, 0, terrainHeight);
    cam = TerrainWalkCamera(pos, Vector3f(1.0f, 1.0f, -0.30f), Vector3f(0, 0, 1), pTerr, 30.0f, 0.05f, GetWindow());
    cam.TerrainScale = Vector3f(terrainBreadth, terrainBreadth, terrainHeight);
    cam.Info.FOV = ToRadian(55.0f);
    cam.Info.zFar = 10000.0f;
    cam.Info.zNear = 1.0f;
    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;


	//Create terrain mesh.
	testMesh = Mesh(PrimitiveTypes::Triangles, 1, &vid);
    Materials::LitTexture_SetUniforms(testMesh, dirLight);
    PassSamplers dummySamplers;
    dummySamplers[0] = imgObj;
    testMesh.TextureSamplers.insert(testMesh.TextureSamplers.end(), dummySamplers);


	//Create post-process effect.
	ClearAllRenderingErrors();
    std::vector<RenderingPass> passes;
    passes.insert(passes.end(), Materials::EmptyPostProcess);
    effect = new PostProcessEffect(windowSize.x, windowSize.y, passes);
	if (effect->HasError())
	{
		std::cout << "Error creating render target: " << effect->GetErrorMessage() << "\n";
		Pause();
		EndWorld();
		return;
	}
    effect->GetMaterial().AddUniform("transparency");
    effect->GetMaterial().AddUniform("isScreenSpace");
    float transparency = 0.3f;
    effect->GetMaterial().SetUniformF("transparency", &transparency, 1);
    int isScreenSpace = 1;
    effect->GetMaterial().SetUniformI("isScreenSpace", &isScreenSpace, 1);
}

OpenGLTestWorld::~OpenGLTestWorld(void)
{
	DeleteAndSetToNull(effect);
	DeleteAndSetToNull(testMat);
	DeleteAndSetToNull(foliage);
    DeleteAndSetToNull(pTerr);
}
void OpenGLTestWorld::OnWorldEnd(void)
{
	DeleteAndSetToNull(effect);
	DeleteAndSetToNull(testMat);
    DeleteAndSetToNull(foliage);
    DeleteAndSetToNull(pTerr);
}

void OpenGLTestWorld::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
	Pause();
}


void OpenGLTestWorld::UpdateWorld(float elapsedSeconds)
{
	if (cam.Update(elapsedSeconds, std::shared_ptr<OculusDevice>(0)))
	{
		EndWorld();
	}
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo & info)
{
	std::vector<const Mesh *> meshes;
	meshes.insert(meshes.begin(), &testMesh);
	if (!testMat->Render(info, meshes))
	{
		std::cout << "Error rendering world: " << testMat->GetErrorMessage() << "\n";
		Pause();
		EndWorld();
        return;
	}
	

	if (!foliage->Render(info))
	{
		std::cout << "Error rendering foliage: " << foliage->GetError() << "\n";
		Pause();
		EndWorld();
	}
}

void OpenGLTestWorld::RenderWorld(float elapsedSeconds)
{
	Matrix4f worldM, viewM, projM;
	TransformObject dummy;

	worldM.SetAsIdentity();
	cam.GetViewTransform(viewM);
	projM.SetAsPerspProj(cam.Info);

	RenderInfo info((SFMLOpenGLWorld*)this, (Camera*)&cam, &dummy, &worldM, &viewM, &projM);
	
    //Set up Post-Process effect if necessary.
	bool should = ShouldUseFramebuffer();
	if (should)
	{
		effect->GetRenderTarget()->EnableDrawingInto();
		if (!effect->GetRenderTarget()->IsValid())
		{
			std::cout << "Error setting up render target.\n";
			Pause();
			EndWorld();
			return;
		}
	}
	
    //Draw the world.
	ScreenClearer().ClearScreen();
	RenderWorldGeometry(info);

    //Now draw the Post-Processed world render if necessary.
	if (should)
	{
        effect->GetRenderTarget()->DisableDrawingInto(windowSize.x, windowSize.y);	
		ScreenClearer().ClearScreen();
        effect->RenderEffect(info);
        if (effect->HasError())
		{
			std::cout << "Error rendering post-process effect.\n";
			Pause();
			EndWorld();
			return;
		}
	}

	GetWindow()->display();
}


void OpenGLTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
	ClearAllRenderingErrors();

	glViewport(0, 0, newW, newH);
	if (!PrintRenderError("Error updating the OpenGL viewport size"))
	{
		return;
	}

    effect->GetRenderTarget()->ChangeSize(newW, newH);
    if (!effect->GetRenderTarget()->IsValid())
	{
        std::cout << "Error changing render target size: " << effect->GetRenderTarget()->GetErrorMessage() << "\n";
		Pause();
		EndWorld();
		return;
	}

	cam.Info.Width = newW;
	cam.Info.Height = newH;
}