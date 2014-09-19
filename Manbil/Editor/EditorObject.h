#pragma once

#include <string>
#include "../Rendering/GUI/GUIElement.h"
#include "EditorMaterialSet.h"


//A (usually modifiable) piece of data to be displayed in the editor.
struct EditorObject
{
public:

    static std::string ErrorMsg;


    EditorObject(void) : activeGUIElement(0) { }
    virtual ~EditorObject(void) { }


    //Gets the currently-active GUIElement.
    GUIElementPtr GetActiveGUIElement(void) const { return activeGUIElement; }

    //Initializes the GUIElement for representing/editing this data.
    //If there was a problem, returns false and sets the static "ErrorMsg" field. Otherwise returns true.
    //Takes in the content to use for rendering.
    virtual bool InitGUIElement(EditorMaterialSet & materialSet) = 0;
    //Updates this object and its element. Should NOT update this object's active GUI element.
    //Default behavior: do nothing.
    virtual void Update(float elapsed, Vector2f panelRelativeMousePos) { }

protected:

    GUIElementPtr activeGUIElement;
};