#include "EditorObjects.h"

#include "../Rendering/GUI/GUI Elements/GUIPanel.h"


std::string EditorObject::ErrorMsg = "";


bool CheckboxValue::InitGUIElement(EditorMaterialSet & materialSet)
{
   MTexture2D *boxTex = &materialSet.CheckBoxBackgroundTex,
              *checkTex = &materialSet.CheckBoxCheckTex;
   GUITexture boxGUITex(materialSet.GetStaticMatParams(boxTex), boxTex,
                        materialSet.GetStaticMaterial(boxTex), false, materialSet.AnimateSpeed),
              checkGUITex(materialSet.GetAnimatedMatParams(checkTex), checkTex,
                          materialSet.GetAnimatedMaterial(checkTex), false, materialSet.AnimateSpeed);
    GUICheckbox * box = new GUICheckbox(materialSet.StaticMatGreyParams, boxGUITex, checkGUITex);

    box->SetChecked(DefaultValue, false);
    box->OnClicked = OnBoxClicked;
    box->OnClicked_Data = OnBoxClicked_Data;

    activeGUIElement = GUIElementPtr(box);
    return true;
}

bool DropdownValues::InitGUIElement(EditorMaterialSet & materialSet)
{
    GUITexture itemListBackground(materialSet.GetStaticMatParams(&materialSet.SelectionBoxBackgroundTex),
                                  &materialSet.SelectionBoxBackgroundTex,
                                  materialSet.GetStaticMaterial(&materialSet.SelectionBoxBackgroundTex),
                                  false, materialSet.AnimateSpeed);
    GUITexture selectedItemBox(materialSet.GetAnimatedMatParams(&materialSet.SelectionBoxBoxTex),
                               &materialSet.SelectionBoxBoxTex,
                               materialSet.GetAnimatedMaterial(&materialSet.SelectionBoxBoxTex),
                               false, materialSet.AnimateSpeed);
    GUISelectionBox* box = new GUISelectionBox(materialSet.StaticMatGreyParams,
                                               &materialSet.TextRender,
                                               selectedItemBox, materialSet.FontID,
                                               Vector2u(selectedItemBox.Tex->GetWidth(),
                                               selectedItemBox.Tex->GetHeight()),
                                               TextureSampleSettings2D(FT_LINEAR, WT_CLAMP),
                                               materialSet.StaticMatColor, GUILabel::HO_LEFT,
                                               itemListBackground, Items, 0, true, materialSet.AnimateSpeed);
    if (box->BoxElement.Mat == 0)
    {
        ErrorMsg = "Error generating text slots for dropdown box: " + box->TextRender->GetError();
        delete box;
        activeGUIElement = GUIElementPtr(0);
        return false;
    }

    activeGUIElement = GUIElementPtr(box);
    return true;
}

bool TextBoxString::InitGUIElement(EditorMaterialSet & materialSet)
{
    if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID, BoxDimensions.x, BoxDimensions.y,
                                                      false, TextureSampleSettings2D(FT_NEAREST, WT_CLAMP)))
    {
        ErrorMsg = "Error creating text render slot: " + materialSet.TextRender.GetError();
        activeGUIElement = GUIElementPtr(0);
        return false;
    }

    TextRenderer::FontSlot slot(materialSet.FontID,
                                materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);
    GUITexture boxBackground(materialSet.GetStaticMatParams(&materialSet.TextBoxBackgroundTex),
                             &materialSet.TextBoxBackgroundTex,
                             materialSet.GetStaticMaterial(&materialSet.TextBoxBackgroundTex),
                             false, materialSet.AnimateSpeed);
    GUILabel boxContents(materialSet.StaticMatColParams, &materialSet.TextRender,
                         slot, materialSet.StaticMatColor, materialSet.AnimateSpeed,
                         GUILabel::HO_LEFT, GUILabel::VO_CENTER);
    GUITextBox * box = new GUITextBox(boxBackground, boxBackground, GUITexture(), boxContents,
                                      (float)BoxDimensions.x, (float)BoxDimensions.y, true,
                                      materialSet.StaticMatGreyParams, materialSet.AnimateSpeed);
    //Try setting the initial value of the box.
    ErrorMsg.clear();
    ErrorMsg = box->SetText(StartingValue);
    if (!ErrorMsg.empty())
    {
        delete box;
        ErrorMsg = "Error setting initial text box value to '" + StartingValue + "': " + ErrorMsg;
        activeGUIElement = GUIElementPtr(0);
        return false;
    }

    //When new text is entered, try to parse it.
    box->OnTextChanged = [](GUITextBox* tBox, void* pData)
    {
        TextBoxString * tbo = (TextBoxString*)pData;

        //Raise the correct event.
        if (tbo->OnValueChanged != 0)
            tbo->OnValueChanged(tBox, tbo->OnValueChanged_Data);
    };
    box->OnTextChanged_Data = this;
    
    activeGUIElement = GUIElementPtr(box);
    return true;
}

bool EditorButton::InitGUIElement(EditorMaterialSet & materialSet)
{
    //First try to create the font slot to render the label.
    if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID,
                                                      (unsigned int)(ButtonSize.x + 1.0f),
                                                      (unsigned int)(ButtonSize.y + 1.0f),
                                                      false, TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        ErrorMsg = "Error creating text render slot for button '" + Text +
                       "'s label: " + materialSet.TextRender.GetError();
        activeGUIElement = GUIElementPtr(0);
        return false;
    }
    TextRenderer::FontSlot labelSlot(materialSet.FontID,
                                     materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);
    //Next try to render the text.
    if (!materialSet.TextRender.RenderString(labelSlot, Text))
    {
        ErrorMsg = "Error render '" + Text + "' into the button's GUILabel: " + materialSet.TextRender.GetError();
        return false;
    }

    buttonTex = GUIElementPtr(new GUITexture(materialSet.GetAnimatedMatParams(&materialSet.ButtonTex),
                                             &materialSet.ButtonTex,
                                             materialSet.GetAnimatedMaterial(&materialSet.ButtonTex),
                                             true, materialSet.AnimateSpeed));
    GUITexture* buttonTexPtr = (GUITexture*)buttonTex.get();
    buttonTexPtr->OnClicked = OnClick;
    buttonTexPtr->OnClicked_pData = OnClick_Data;

    buttonLabel = GUIElementPtr(new GUILabel(materialSet.StaticMatColParams, &materialSet.TextRender,
                                             labelSlot, materialSet.StaticMatColor,
                                             materialSet.AnimateSpeed, GUILabel::HO_LEFT, GUILabel::VO_CENTER));

    GUIPanel* panel = new GUIPanel(materialSet.StaticMatGreyParams, ButtonSize, materialSet.AnimateSpeed);
    panel->AddElement(buttonTex);
    panel->AddElement(buttonLabel);
    activeGUIElement = GUIElementPtr(panel);
    return true;
}