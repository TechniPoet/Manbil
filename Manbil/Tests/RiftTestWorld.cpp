#include "RiftTestWorld.h"


#include "../Rendering/Primitives/PrimitiveGenerator.h"
#include "../DebugAssist.h"
#include "../Rendering/Basic Rendering/MaterialData.h"

#include <OVR_CAPI_GL.h>
#include <OVR.h>


RiftTestWorld::RiftTestWorld(void)
    : objectMat(0), quadMat(0),
      floorTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_8U, true),
      obj1Tex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_8U, true),
      obj2Tex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_8U, true),
      eyeDepthTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_32F_DEPTH, false),
      eyeColorTex1(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_32F, false),
      eyeColorTex2(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_32F, false),
      lightCol(1.0f, 1.0f, 1.0f), lightDir(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
      ambientLight(0.35f), diffuseLight(0.65f), specLight(1.0f), specIntensity(256.0f),
      baseCam(Vector3f(1.0f, 1.0f, 2.5f), 2.0f, 0.15f,
              Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f),
              true),
      windowSize(800, 600),
      floorT(Vector3f(0.0f, 0.0f, -1.0f), Vector3f(), Vector3f(10.0f, 10.0f, 1.0f)),
      obj1T(Vector3f(3.0f, 3.0f, 1.5f), Vector3f(), Vector3f(0.25f, 0.25f, 0.25f)),
      obj2T(Vector3f(-2.0f, -2.0f, 1.5f), Vector3f(), Vector3f(0.25f, 0.25f, 0.25f)),
      SFMLOpenGLWorld(1920, 1080, sf::ContextSettings(24, 0, 0, 4, 1))
{

}


void RiftTestWorld::InitializeOculus(void)
{
    OculusDevice::InitializeSystem();

    //Set up OVR HMD.
    if (ovrHmd_Detect() > 0)
    {
        std::cout << "Rift HMD was found!\n";
        hmd = ovrHmd_Create(0);

        std::cout << "Manufacturer: " << hmd->Manufacturer << "\n" <<
                     "Serial number: " << hmd->SerialNumber << "\n" <<
                     "Display device name: " << hmd->DisplayDeviceName << "\n\n";

        bool b = (bool)ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
                                                     ovrTrackingCap_MagYawCorrection |
                                                     ovrTrackingCap_Position, 0);
        if (!Assert(b, "Error configuring Rift headset for tracking", ovrHmd_GetLastError(hmd)))
        {
            return;
        }
    }
}
void RiftTestWorld::InitializeTextures(void)
{
    //Object textures.

    floorTex.Create();
    obj1Tex.Create();
    obj2Tex.Create();

    std::string err;
    if (!Assert(floorTex.SetDataFromFile("Content/Textures/Grass.png", err),
                "Error loading floor tex 'Grass.png'", err))
    {
        return;
    }
    if (!Assert(obj1Tex.SetDataFromFile("Content/Textures/VoxelTex.png", err),
                "Error loading obj1 tex 'VoxelTex.png'", err))
    {
        return;
    }
    if (!Assert(obj2Tex.SetDataFromFile("Content/Textures/Sky.png", err),
                "Error loading obj2 tex 'Sky.png'", err))
    {
        return;
    }


    //Render target textures.

    eyeColorTex1.Create();
    eyeColorTex2.Create();
    eyeDepthTex.Create();

    eyeRendTarget = RenderTargets.CreateRenderTarget(eyeDepthTex.GetPixelSize(), err);
    if (!Assert(eyeRendTarget != RenderTargetManager::ERROR_ID,
                "Error creating eye render target", err))
    {
        return;
    }
    RenderTargets[eyeRendTarget]->SetDepthAttachment(RenderTargetTex(&eyeDepthTex), false);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RiftTestWorld::InitializeMaterials(void)
{
    #pragma region Quad mat

    //Create shaders.
    std::string vertShader = "\
#version 400            \n\
\n\
layout (location = 0) in vec3 in_pos;            \n\
layout (location = 1) in vec2 in_uv;            \n\
layout (location = 2) in vec3 in_normal;            \n\
\n\
out vec2 out_uv;            \n\
\n\
void main()            \n\
{            \n\
    gl_Position = vec4(in_pos, 1.0f);        \n\
    out_uv = in_uv;        \n\
}";
    std::string fragShader = "\
#version 400        \n\
\n\
in vec2 out_uv;        \n\
\n\
out vec4 finalColor4;        \n\
\n\
uniform sampler2D u_tex;        \n\
\n\
void main()        \n\
{        \n\
    finalColor4 = texture2D(u_tex, out_uv);    \n\
}";

    //Initialize parameters.
    quadParams.Texture2Ds["u_tex"] = UniformValueSampler2D(0, "u_tex");

    //Create the material.
    std::string err;
    quadMat = new Material(vertShader, fragShader, quadParams,
                           DrawingQuad::GetVertexInputData(), BlendMode::GetOpaque(),
                           err);
    if (!Assert(err.empty(), "Error creating quad mat", err))
    {
        return;
    }

    #pragma endregion
    

    #pragma region Object mat

    //Create shaders.

    vertShader = "\
#version 400            \n\
\n\
layout (location = 0) in vec3 in_pos;            \n\
layout (location = 1) in vec2 in_uv;            \n\
layout (location = 2) in vec3 in_normal;            \n\
\n\
out vec2 out_uv;            \n\
out vec3 out_worldPos,            \n\
         out_worldNormal;            \n\
\n\
uniform mat4 " + MaterialConstants::WVPMatName + ";            \n\
uniform mat4 " + MaterialConstants::WorldMatName + ";            \n\
\n\
void main()            \n\
{            \n\
    vec4 screenPos4 = " + MaterialConstants::WVPMatName + " * vec4(in_pos, 1.0f);            \n\
    vec4 worldPos4 = " + MaterialConstants::WorldMatName + " * vec4(in_pos, 1.0f);            \n\
\n\
    out_worldPos = worldPos4.xyz / worldPos4.w;            \n\
    out_worldNormal = normalize((" + MaterialConstants::WorldMatName + " * vec4(in_normal, 0.0f)).xyz);            \n\
    out_uv = in_uv;         \n\
\n\
    gl_Position = screenPos4;            \n\
}";

    fragShader = "\
#version 400            \n\
\n\
in vec2 out_uv;            \n\
in vec3 out_worldPos, out_worldNormal;            \n\
\n\
out vec4 finalOutColor;            \n\
\n\
uniform sampler2D u_tex;            \n\
uniform float u_ambientLight, u_diffuseLight, u_specularLight;            \n\
uniform float u_specularIntensity;            \n\
uniform vec3 u_lightCol, u_lightDir;            \n\
uniform float u_texScale;                   \n\
uniform vec3 " + MaterialConstants::CameraPosName + ";            \n\
\n\
void main()            \n\
{            \n\
    vec3 texSample = texture2D(u_tex, out_uv * u_texScale).xyz;            \n\
\n\
    vec3 worldNormal = normalize(out_worldNormal);            \n\
    float lightDot = max(dot(worldNormal, -u_lightDir), 0.0f);            \n\
    vec3 reflectedLight = normalize(reflect(u_lightDir, worldNormal));            \n\
    float specFactor = max(0.0f, dot(reflectedLight, normalize(" + MaterialConstants::CameraPosName + " - out_worldPos)));            \n\
    specFactor = pow(specFactor, u_specularIntensity);            \n\
    vec3 finalLight = u_lightCol * (u_ambientLight + (u_diffuseLight * lightDot) + (u_specularLight * specFactor));            \n\
\n\
    finalOutColor = vec4(finalLight * texSample, 1.0f);            \n\
}";


    //Initialize parameters.
    UniformDictionary params;
    params.Floats["u_ambientLight"] = UniformValueF(ambientLight, "u_ambientLight");
    params.Floats["u_diffuseLight"] = UniformValueF(diffuseLight, "u_diffuseLight");
    params.Floats["u_specularLight"] = UniformValueF(specLight, "u_specularLight");
    params.Floats["u_specularIntensity"] = UniformValueF(specIntensity, "u_specularIntensity");
    params.Floats["u_lightCol"] = UniformValueF(lightCol, "u_lightCol");
    params.Floats["u_lightDir"] = UniformValueF(lightDir, "u_lightDir");
    params.Floats["u_texScale"] = UniformValueF(1.0f, "u_texScale");
    params.Texture2Ds["u_tex"] = UniformValueSampler2D(0, "u_tex");

    //Create the material.
    objectMat = new Material(vertShader, fragShader, params,
                             VertexPosUVNormal::GetAttributeData(),
                             BlendMode::GetOpaque(), err);
    if (!Assert(err.empty(), "Error creating object material", err))
    {
        return;
    }

    floorParams = params;
    floorParams.Texture2Ds["u_tex"].Texture = floorTex.GetTextureHandle();
    floorParams.Floats["u_texScale"].SetValue(4.0f);
    obj1Params = params;
    obj1Params.Texture2Ds["u_tex"].Texture = obj1Tex.GetTextureHandle();
    obj2Params = params;
    obj2Params.Texture2Ds["u_tex"].Texture = obj2Tex.GetTextureHandle();

    #pragma endregion
}
void RiftTestWorld::InitializeObjects(void)
{
    DrawingQuad::InitializeQuad();


    std::vector<VertexPosUVNormal> verts;
    std::vector<unsigned int> indices;
    PrimitiveGenerator::GenerateCube(verts, indices, false, false);

    cube.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));
    MeshData& dat = cube.SubMeshes[0];
    dat.SetVertexData(verts, MeshData::BUF_STATIC, VertexPosUVNormal::GetAttributeData());
    dat.SetIndexData(indices, MeshData::BUF_STATIC);
}
void RiftTestWorld::SetUpOculusDevice(void)
{
    if (hmd != NULL)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowSize.x, windowSize.y);

        ovrHmd_SetEnabledCaps(hmd, ovrHmd_GetEnabledCaps(hmd) | ovrHmdCap_ExtendDesktop |
                                   ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);
        if (!(hmd->HmdCaps & ovrHmdCap_ExtendDesktop))
        {
            ovrHmd_AttachToWindow(hmd, GetWindow()->getSystemHandle(), 0, 0);
        }

        for (int eye = 0; eye < 2; ++eye)
        {
            ovrSizei idealSize = ovrHmd_GetFovTextureSize(hmd, (ovrEyeType)eye,
                                                          hmd->DefaultEyeFov[eye], 1.0f);
            MTexture2D& eyeTex = (eye == 0 ? eyeColorTex1 : eyeColorTex2);
            eyeTex.SetColorData(Array2D<Vector4b>((unsigned int)idealSize.w,
                                                  (unsigned int)idealSize.h));

        }
        assert(eyeColorTex1.GetWidth() == eyeColorTex2.GetWidth());
        assert(eyeColorTex1.GetHeight() == eyeColorTex2.GetHeight());
        if (!Assert(RenderTargets[eyeRendTarget]->UpdateSize(),
                    "Render targets are too big!", ""))
        {
            return;
        }


        ovrGLConfig config;
        config.OGL.Header.API = ovrRenderAPI_OpenGL;
        config.OGL.Header.BackBufferSize.w = hmd->Resolution.w;
        config.OGL.Header.BackBufferSize.h = hmd->Resolution.h;
        config.OGL.Header.Multisample = 1;
        config.OGL.Window = GetWindow()->getSystemHandle();
        config.OGL.DC = GetDC(config.OGL.Window);
        unsigned int distortionCaps = ovrDistortionCap_Chromatic |
                                      ovrDistortionCap_Overdrive |
                                      ovrDistortionCap_Vignette |
                                      ovrDistortionCap_TimeWarp;
        bool b = (bool)ovrHmd_ConfigureRendering(hmd, &config.Config,
                                                 distortionCaps, hmd->DefaultEyeFov, hmdEyeSettings);
        if (!b)
        {
            Assert(false, "Error configuring Rift rendering", ovrHmd_GetLastError(hmd));
            return;
        }

        std::cout << "Rendering configured.\nCorrects chromatic issues: " <<
                         (bool)(hmd->DistortionCaps & ovrDistortionCap_Chromatic) << "\n" <<
                     "Uses timewarp: " <<
                         (bool)(hmd->DistortionCaps & ovrDistortionCap_TimeWarp) << "\n" <<
                     "Uses vignette: " <<
                         (bool)(hmd->DistortionCaps & ovrDistortionCap_Vignette) << "\n" <<
                     "Uses overdrive: " <<
                         (bool)(hmd->DistortionCaps & ovrDistortionCap_Overdrive) << "\n\n";

        //Set up textures.
        ovrTexes[0].OGL.Header.API = ovrRenderAPI_OpenGL;
        ovrTexes[0].OGL.Header.RenderViewport.Pos.x = 0;
        ovrTexes[0].OGL.Header.RenderViewport.Pos.y = 0;
        ovrTexes[0].OGL.Header.RenderViewport.Size = ovrHmd_GetFovTextureSize(hmd, (ovrEyeType)0,
                                                                              hmd->DefaultEyeFov[0], 1.0f);
        ovrTexes[0].OGL.Header.TextureSize.w = eyeColorTex1.GetWidth();
        ovrTexes[0].OGL.Header.TextureSize.h = eyeColorTex1.GetHeight();
        ovrTexes[0].OGL.TexId = eyeColorTex1.GetTextureHandle();

        ovrTexes[1].OGL.Header.API = ovrRenderAPI_OpenGL;
        ovrTexes[1].OGL.Header.RenderViewport.Pos.x = 0;
        ovrTexes[1].OGL.Header.RenderViewport.Pos.y = 0;
        ovrTexes[1].OGL.Header.RenderViewport.Size = ovrHmd_GetFovTextureSize(hmd, (ovrEyeType)1,
                                                                              hmd->DefaultEyeFov[1], 1.0f);
        ovrTexes[1].OGL.Header.TextureSize.w = eyeColorTex2.GetWidth();
        ovrTexes[1].OGL.Header.TextureSize.h = eyeColorTex2.GetHeight();
        ovrTexes[1].OGL.TexId = eyeColorTex2.GetTextureHandle();
    }
    else
    {
        std::cout << "No Rift HMD was found.\n\n";
        char dummy;
        std::cin >> dummy;

        EndWorld();
        return;
    }

    windowSize = Vector2u((unsigned int)hmd->Resolution.w, (unsigned int)hmd->Resolution.h);
}

void RiftTestWorld::InitializeWorld(void)
{
    InitializeOculus();


    SFMLOpenGLWorld::InitializeWorld();
    if (IsGameOver())
    {
        return;
    }

    GetWindow()->setVerticalSyncEnabled(false);
    GetWindow()->setPosition(sf::Vector2i(1920, 0));


    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();
    if (IsGameOver())
    {
        return;
    }


    SetUpOculusDevice();
    GetWindow()->setPosition(sf::Vector2i(1920, 0));


    baseCam.Window = GetWindow();
    baseCam.PerspectiveInfo.zNear = 0.1f;
    baseCam.PerspectiveInfo.zFar = 1000.0f;
    baseCam.PerspectiveInfo.Width = windowSize.x;
    baseCam.PerspectiveInfo.Height = windowSize.y;
    baseCam.PerspectiveInfo.SetFOVDegrees(55.0f);
    

    GetWindow()->setPosition(sf::Vector2i(0, 0));
    GetWindow()->setSize(sf::Vector2u(windowSize.x, windowSize.y));


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowSize.x, windowSize.y);
}
void RiftTestWorld::OnWorldEnd(void)
{
    DrawingQuad::DestroyQuad();

    delete objectMat;
    objectMat = 0;
    delete quadMat;
    quadMat = 0;

    floorTex.DeleteIfValid();
    obj1Tex.DeleteIfValid();
    obj2Tex.DeleteIfValid();

    eyeColorTex1.DeleteIfValid();
    eyeColorTex2.DeleteIfValid();
    eyeDepthTex.DeleteIfValid();

    if (hmd != 0)
    {
        ovrHmd_Destroy(hmd);
    }

    OculusDevice::DestroySystem();
}

void RiftTestWorld::UpdateWorld(float elapsedSeconds)
{
    ovrTiming = ovrHmd_BeginFrame(hmd, 0);

    baseCam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }

    //Update each param list.
    for (int i = 0; i < 3; ++i)
    {
        UniformDictionary& d = (i == 0 ? floorParams : (i == 1 ? obj1Params : obj2Params));
        d.Floats["u_ambientLight"].SetValue(ambientLight);
        d.Floats["u_diffuseLight"].SetValue(diffuseLight);
        d.Floats["u_specularLight"].SetValue(specLight);
        d.Floats["u_specularIntensity"].SetValue(specIntensity);
        d.Floats["u_lightCol"].SetValue(lightCol);
        d.Floats["u_lightDir"].SetValue(lightDir);
    }
}

void RiftTestWorld::RenderWorldGeometry(const RenderInfo& info)
{
    RenderingState(RenderingState::C_NONE).EnableState();
    ScreenClearer(true, true, false, Vector4f(0.2f, 0.2f, 0.2f, 0.0f)).ClearScreen();
    BlendMode::GetOpaque().EnableMode();

    //Floor.
    cube.Transform = floorT;
    objectMat->Render(info, &cube, floorParams);

    //Object 1.
    cube.Transform = obj1T;
    objectMat->Render(info, &cube, obj1Params);

    //Object 2.
    cube.Transform = obj2T;
    objectMat->Render(info, &cube, obj2Params);

}
void RiftTestWorld::RenderOpenGL(float elapsedSeconds)
{
    Matrix4f viewM, projM;

    
    //Update OVR/camera state and render.

    static_assert(sizeof(Vector3f) == sizeof(ovrVector3f),
                  "Vector3f should be the same size as ovrVector3f!");

    ovrVector3f useHmdToEyeViewOffset[2] = { hmdEyeSettings[0].HmdToEyeViewOffset,
			                                 hmdEyeSettings[1].HmdToEyeViewOffset };
    ovrPosef eyeRenderPose[2];
    ovrHmd_GetEyePoses(hmd, 0, useHmdToEyeViewOffset, eyeRenderPose, 0);

    
    //Get the base camera's yaw.
    float yaw = Quaternion(Vector3f(1.0f, 0.0f, 0.0f), baseCam.GetForward()).GetEulerAngles().z;
    baseCam.SetRotation(Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
    baseCam.AddYaw(yaw);


    for (unsigned int eyeIndex = 0; eyeIndex < ovrEye_Count; ++eyeIndex)
    {
        //Get eye data.

        ovrEyeType eye = hmd->EyeRenderOrder[eyeIndex];

        MTexture2D& rendTex = (eyeIndex == 0 ? eyeColorTex1 : eyeColorTex2);
        if (!Assert(RenderTargets[eyeRendTarget]->SetColorAttachment(RenderTargetTex(&rendTex), true),
                    "Error changing render target attachment", "Unknown"))
        {
            return;
        }

        ovrPosef& eyePose = eyeRenderPose[eye];


        //Set up the camera.
        Camera eyeCam(baseCam);
        baseCam.LockUp = false;
        OVR::Quatf ovrOrientation = eyePose.Orientation;

        //Get the axis/angle of rotation from the headset.
        Vector3f axisTemp;
        float angle;
        ovrOrientation.GetAxisAngle((OVR::Vector3f*)&axisTemp, &angle);

        //Convert it to a rotation in this engine's coordinate system.
        Vector3f axis(axisTemp.z, -axisTemp.x, -axisTemp.y);
        Quaternion rotQuat(axis, angle);
        rotQuat = Quaternion(rotQuat, Quaternion(Vector3f(0.0f, 0.0f, 1.0f), yaw));

        //Rotate the camera.
        eyeCam.SetRotation(Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
        eyeCam.Rotate(rotQuat);

        //Translate the camera.
        Vector3f eyePos(-eyePose.Position.z, eyePose.Position.x, eyePose.Position.y);
        eyeCam.IncrementPosition(eyePos);

        //Set up the camera's projection info.
        float fov = 2.0f * atanf(hmdEyeSettings[eye].Fov.UpTan);
        eyeCam.PerspectiveInfo.FOV = fov;
        eyeCam.PerspectiveInfo.Width = rendTex.GetWidth();
        eyeCam.PerspectiveInfo.Height = rendTex.GetHeight();

        eyeCam.GetViewTransform(viewM);
        eyeCam.GetPerspectiveTransform(projM);

        //Render the world.
        RenderTargets[eyeRendTarget]->EnableDrawingInto();
        glViewport(0, 0, rendTex.GetWidth(), rendTex.GetHeight());
        RenderInfo info(GetTotalElapsedSeconds(), &eyeCam, &viewM, &projM);
        RenderWorldGeometry(info);
        RenderTargets[eyeRendTarget]->DisableDrawingInto();
    }

    //Output rendered eye data to the SDK for display.
    glViewport(0, 0, windowSize.x, windowSize.y);
    ovrHmd_EndFrame(hmd, eyeRenderPose, &ovrTexes[0].Texture);
    GetWindow()->setPosition(sf::Vector2i(1920, 0));
}

void RiftTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    glViewport(0, 0, newW, newH);

    windowSize.x = newW;
    windowSize.y = newH;

    baseCam.PerspectiveInfo.Width = (float)newW;
    baseCam.PerspectiveInfo.Height = (float)newH;
}
void RiftTestWorld::OnInitializeError(std::string errorMsg)
{
    EndWorld();

    SFMLOpenGLWorld::OnInitializeError(errorMsg);

    std::cout << "Enter any key to continue: ";
    char dummy;
    std::cin >> dummy;
}

bool RiftTestWorld::Assert(bool val, std::string errorIntro, const std::string& error)
{
    if (!val)
    {
        std::cout << errorIntro << ": " << error << "\n";

        std::cout << "Enter anything to continue: ";
        char dummy;
        std::cin >> dummy;

        EndWorld();
    }
    return val;
}