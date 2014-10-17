#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include "../Vertices.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"

#include "../ScreenClearer.h"
#include "../RenderingState.h"
#include "../Editor/EditorPanel.h"
#include "../Editor/EditorObjects.h"

#include "../DebugAssist.h"


//Debugging/error-printing.
#include <iostream>
namespace GUITESTWORLD_NAMESPACE
{
    void Pause(void)
    {
        char dummy;
        std::cin >> dummy;
    }
    bool PrintError(bool whetherToPrint, std::string errorIntro, std::string errorMsg)
    {
        if (whetherToPrint)
        {
            std::cout << errorIntro << ": " << errorMsg << "\n";
        }

        return whetherToPrint;
    }
}
using namespace GUITESTWORLD_NAMESPACE;


Vector2i GUITestWorld::WindowSize = Vector2i();


bool GUITestWorld::ReactToError(bool isEverythingOK, std::string errorIntro, std::string errorMsg)
{
    if (PrintError(!isEverythingOK, errorIntro, errorMsg))
    {
        Pause();
        EndWorld();
    }

    return isEverythingOK;
}
void GUITestWorld::OnInitializeError(std::string errorMsg)
{
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    Pause();
    EndWorld();
}


void GUITestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    WindowSize.x = newW;
    WindowSize.y = newH;
}


void GUITestWorld::InitializeWorld(void)
{
    std::string err;


    SFMLOpenGLWorld::InitializeWorld();

    //Initialize static stuff.
    err = InitializeStaticSystems(false, true, true);
    if (!ReactToError(err.empty(), "Error initializing static systems", err))
        return;


    //Set up editor content.
    editorMaterials = new EditorMaterialSet(*TextRender);
    err = EditorMaterialSet::GenerateDefaultInstance(*editorMaterials);
    if (!ReactToError(err.empty(), "Error loading default editor materials", err))
    {
        delete editorMaterials;
        editorMaterials = 0;
        return;
    }

    //Build the editor.
    EditorPanel* editor = new EditorPanel(*editorMaterials, 0.0f, 0.0f);
    if (false)
    {
        editor->AddObject(EditorObjectPtr(new CheckboxValue(EditorObject::DescriptionData("Check this shit"), Vector2f(), false)));
        editor->AddObject(EditorObjectPtr(new TextBoxUInt(56, Vector2u(200, 50), Vector2f(),
                                                          EditorObject::DescriptionData("Type a uint!"),
                                                          [](GUITextBox* textBox, unsigned int newVal, void* pData)
                                                            { std::cout << newVal << "\n"; })));
        editor->AddObject(EditorObjectPtr(new EditorButton("Push me", Vector2f(200.0f, 50.0f), Vector2f(),
                                                           EditorObject::DescriptionData("<-- Push this shit", false, 30.0f),
                                                           [](GUITexture* clicked, Vector2f mp, void* pDat)
                                                            { std::cout << "Clicked\n"; })));
    }
    else
    {
        std::vector<EditorObjectPtr> ptrs;
        colEd.Color = Vector4f(0.5f, 0.25f, 1.0f, 0.5f);

        std::string err = colEd.BuildEditorElements(ptrs, *editorMaterials);
        if (!ReactToError(err.empty(), "Error building color editor elements", err))
            return;

        for (unsigned int i = 0; i < ptrs.size(); ++i)
        {
            err = editor->AddObject(ptrs[i]);
            if (!ReactToError(err.empty(), "Error adding ptr element #" + std::to_string(i + 1), err))
                return;
        }
    }
    guiManager = GUIManager(GUIElementPtr(editor));

    //Set up the window.
    Vector2f dims = editor->GetCollisionDimensions();
    WindowSize = Vector2i((int)dims.x, (int)dims.y);
    //WindowSize = Vector2i(500, 500);
    editor->SetPosition(dims * 0.5f);
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    GetWindow()->setSize(sf::Vector2u(WindowSize.x, WindowSize.y));
}
void GUITestWorld::DestroyMyStuff(bool destroyStatics)
{
    DeleteAndSetToNull(editorMaterials);

    if (destroyStatics) DestroyStaticSystems(false, true, true);
}


void GUITestWorld::UpdateWorld(float elapsed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        EndWorld();
    
    //Move the gui window.
    const float speed = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        guiManager.RootElement->MoveElement(Vector2f(-(speed * elapsed), 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        guiManager.RootElement->MoveElement(Vector2f((int)(speed * elapsed), 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        guiManager.RootElement->MoveElement(Vector2f(0.0f, (speed * elapsed)));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        guiManager.RootElement->MoveElement(Vector2f(0.0f, -(speed * elapsed)));

    sf::Vector2i mPos = sf::Mouse::getPosition();
    sf::Vector2i mPosFinal = mPos - GetWindow()->getPosition() - sf::Vector2i(5, 30);
    mPosFinal.y = WindowSize.y - mPosFinal.y;

    guiManager.Update(elapsed, Vector2i(mPosFinal.x, mPosFinal.y), sf::Mouse::isButtonPressed(sf::Mouse::Left));
    guiManager.RootElement->SetPosition(Vector2f(WindowSize.x * 0.5f, WindowSize.y * 0.5f));
}
void GUITestWorld::RenderOpenGL(float elapsed)
{
    //Prepare the back-buffer to be rendered into.
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    ScreenClearer(true, true, false, Vector4f(0.1f, 0.1f, 0.1f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE, RenderingState::BE_SOURCE_ALPHA, RenderingState::BE_ONE_MINUS_SOURCE_ALPHA,
                   false, false, RenderingState::AT_GREATER, 0.0f).EnableState();

    //Set up the "render info" struct.
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(0.0f, 0.0f, -10.0f);
    cam.MaxOrthoBounds = Vector3f((float)WindowSize.x, (float)WindowSize.y, 10.0f);
    cam.Info.Width = WindowSize.x;
    cam.Info.Height = WindowSize.y;
    Matrix4f worldM, viewM, projM;
    TransformObject trns;
    trns.GetWorldTransform(worldM);
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);
    RenderInfo info(this, &cam, &trns, &worldM, &viewM, &projM);

    //Render the GUI.
    std::string err = guiManager.Render(elapsed, info);
    if (!ReactToError(err.empty(), "Error rendering GUI", err))
        return;
}