#include "EditorObjects.h"

#include "../Rendering/GUI/GUI Elements/GUIPanel.h"


std::string EditorObject::ErrorMsg = "";


bool CheckboxValue::InitGUIElement(EditorMaterialSet & materialSet)
{
   MTexture2D *boxTex = &materialSet.CheckBoxBackgroundTex,
              *checkTex = &materialSet.CheckBoxCheckTex;
   GUITexture boxGUITex(materialSet.GetAnimatedMatParams(boxTex), boxTex,
                        materialSet.GetAnimatedMaterial(boxTex), true, materialSet.AnimateSpeed),
              checkGUITex(materialSet.GetAnimatedMatParams(checkTex), checkTex,
                          materialSet.GetAnimatedMaterial(checkTex), false, materialSet.AnimateSpeed);
    GUICheckbox * box = new GUICheckbox(materialSet.StaticMatGreyParams, boxGUITex, checkGUITex);

    box->SetChecked(DefaultValue, false);
    box->OnClicked = OnBoxClicked;
    box->OnClicked_Data = OnBoxClicked_Data;

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(box);
    }
    else
    {
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(box));
        if (activeGUIElement.get() == 0)
            return false;
    }
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
                                               materialSet.StaticMatText, GUILabel::HO_LEFT,
                                               itemListBackground, Items, 0, true, materialSet.AnimateSpeed);
    if (box->BoxElement.Mat == 0)
    {
        ErrorMsg = "Error generating text slots for dropdown box: " + box->TextRender->GetError();
        delete box;
        activeGUIElement = GUIElementPtr(0);
        return false;
    }

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(box);
    }
    else
    {
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(box));
        if (activeGUIElement.get() == 0)
            return false;
    }
    return true;
}

bool EditorButton::InitGUIElement(EditorMaterialSet & materialSet)
{
    //First try to create the font slot to render the label.
    if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID,
                                                      (unsigned int)(ButtonSize.x / materialSet.TextScale.x),
                                                      materialSet.TextRenderSpaceHeight,
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

    //Create the button.
    buttonTex = GUIElementPtr(new GUITexture(materialSet.GetAnimatedMatParams(&materialSet.ButtonTex),
                                             &materialSet.ButtonTex,
                                             materialSet.GetAnimatedMaterial(&materialSet.ButtonTex),
                                             true, materialSet.AnimateSpeed));
    buttonTex->SetBounds(ButtonSize * -0.5f, ButtonSize * 0.5f);
    GUITexture* buttonTexPtr = (GUITexture*)buttonTex.get();
    buttonTexPtr->OnClicked = OnClick;
    buttonTexPtr->OnClicked_pData = OnClick_Data;

    //Create the label.
    buttonLabel = GUIElementPtr(new GUILabel(materialSet.StaticMatTextParams, &materialSet.TextRender,
                                             labelSlot, materialSet.StaticMatText,
                                             materialSet.AnimateSpeed, GUILabel::HO_CENTER, GUILabel::VO_CENTER));
    buttonLabel->SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
    buttonLabel->ScaleBy(materialSet.TextScale);

    GUIPanel* panel = new GUIPanel(materialSet.StaticMatGreyParams, ButtonSize, materialSet.AnimateSpeed);
    panel->AddElement(buttonTex);
    panel->AddElement(buttonLabel);


    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(panel);
    }
    else
    {
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(panel));
        if (activeGUIElement.get() == 0)
            return false;
    }
    return true;
}

bool EditorLabel::InitGUIElement(EditorMaterialSet & materialSet)
{
    //First try to create the font slot to render the label.
    if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID,
                                                      TextRenderSpaceWidth,
                                                      materialSet.TextRenderSpaceHeight,
                                                      false, TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        ErrorMsg = "Error creating text render slot for label '" + Text +
                       "': " + materialSet.TextRender.GetError();
        activeGUIElement = GUIElementPtr(0);
        return false;
    }
    TextRenderer::FontSlot labelSlot(materialSet.FontID,
                                     materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);

    activeGUIElement = GUIElementPtr(new GUILabel(materialSet.StaticMatTextParams, &materialSet.TextRender,
                                                  labelSlot, materialSet.StaticMatText,
                                                  materialSet.AnimateSpeed, GUILabel::HO_CENTER, GUILabel::VO_CENTER));
    activeGUIElement->SetColor(materialSet.TextColor);
    activeGUIElement->ScaleBy(materialSet.TextScale);

    return true;
}

bool EditorImage::InitGUIElement(EditorMaterialSet & set)
{
    GUITexture* texElement = new GUITexture(set.GetStaticMatParams(Tex), Tex,
                                            set.GetStaticMaterial(Tex), false,
                                            set.AnimateSpeed);
    texElement->ScaleBy(Scale);

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(texElement);
    }
    else
    {
        activeGUIElement = AddDescription(set, GUIElementPtr(texElement));
        if (activeGUIElement.get() == 0)
            return false;
    }
    return true;
}

bool CollapsibleEditorBranch::InitGUIElement(EditorMaterialSet & set)
{
    MTexture2D* titleTex = &set.CollapsibleEditorTitleBarTex;
    GUITexture* titleBar = new GUITexture(set.GetAnimatedMatParams(titleTex), titleTex,
                                          set.GetAnimatedMaterial(titleTex), true, set.AnimateSpeed);
    titleBar->OnClicked_pData = this;
    titleBar->OnClicked = [](GUITexture* titleButton, Vector2f mousePos, void* pData)
    {
        CollapsibleEditorBranch* thisB = (CollapsibleEditorBranch*)pData;
        thisB->Toggle();
    };
    GUIElementPtr titleBarPtr(titleBar);


    //First try to create the font slot to render the label.
    if (!set.TextRender.CreateTextRenderSlots(set.FontID, titleTex->GetWidth(),
                                              set.TextRenderSpaceHeight, false,
                                              TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        ErrorMsg = "Error creating text render slot for description '" +
                       DescriptionLabel.Text + "': " + set.TextRender.GetError();
        return false;
    }
    TextRenderer::FontSlot labelSlot(set.FontID, set.TextRender.GetNumbSlots(set.FontID) - 1);
    //Next try to render the text.
    if (!set.TextRender.RenderString(labelSlot, DescriptionLabel.Text))
    {
        ErrorMsg = "Error rendering '" + DescriptionLabel.Text + "' into the description label: " +
                       set.TextRender.GetError();
        return false;
    }
    //Make the label.
    GUIElementPtr label(new GUILabel(set.StaticMatTextParams, &set.TextRender,
                                     labelSlot, set.StaticMatText, set.AnimateSpeed,
                                     GUILabel::HO_RIGHT, GUILabel::VO_CENTER));
    label->SetColor(set.CollapsibleEditorTitleTextCol);
    label->ScaleBy(set.TextScale);
    label->Depth += 0.001f;


    //Combine the title bar and label together into a panel.
    GUIPanel* titleBarPanel = new GUIPanel(UniformDictionary(), titleBar->GetCollisionDimensions().x);
    titleBarPanel->AddElement(titleBarPtr);
    titleBarPanel->AddElement(label);
    label->SetPosition(Vector2f(titleBarPanel->GetCollisionDimensions().x * 0.5f, 0.0f));
    barOnly = GUIElementPtr(titleBarPanel);


    //Now make an outer panel that includes the inner element.
    GUIFormattedPanel* fullPanelPtr = new GUIFormattedPanel(UniformDictionary());
    fullPanelPtr->AddObject(GUIFormatObject(barOnly, false, true, Vector2f(panelIndent, 0.0f)));
    fullPanelPtr->AddObject(GUIFormatObject(innerElement));
    fullPanel = GUIElementPtr(fullPanelPtr);


    //Start with just the title bar.
    activeGUIElement = barOnly;
    return true;
}
bool CollapsibleEditorBranch::Update(float elapsed, Vector2f panelRelMouse)
{
    bool b = didActiveElementChange;
    didActiveElementChange = false;
    return b;
}
void CollapsibleEditorBranch::Toggle(void)
{
    didActiveElementChange = true;

    if (activeGUIElement.get() == barOnly.get())
    {
        activeGUIElement = fullPanel;
    }
    else
    {
        assert(activeGUIElement.get() == fullPanel.get());
        activeGUIElement = barOnly;
    }
}