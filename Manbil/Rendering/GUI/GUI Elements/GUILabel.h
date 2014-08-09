#pragma once

#include "../GUIElement.h"
#include "../TextRenderer.h"
#include "../../Materials/Data Nodes/ShaderGenerator.h"


//Represents a simple piece of text.
class GUILabel : public GUIElement
{
public:

    
    enum HorizontalOffsets
    {
        HO_LEFT,
        HO_CENTER,
        HO_RIGHT,
    };
    enum VerticalOffsets
    {
        VO_TOP,
        VO_CENTER,
        VO_BOTTOM,
    };


    HorizontalOffsets OffsetHorz;
    VerticalOffsets OffsetVert;

    TextRenderer * TextRender;
    TextRenderer::FontSlot TextRenderSlot;

    Material * RenderMat;
    std::string TextSamplerName;

    Vector2f Scale;


    GUILabel(TextRenderer * textRenderer, TextRenderer::FontSlot textSlot, Material * material,
             std::string textSamplerName, std::string _text = "",
             HorizontalOffsets offsetH = HO_LEFT, VerticalOffsets offsetV = VO_TOP)
        : OffsetHorz(offsetH), OffsetVert(offsetV), TextRender(textRenderer),
          TextSamplerName(textSamplerName),
          RenderMat(material), TextRenderSlot(textSlot), text(_text)
    {

    }


    const std::string & GetText(void) const { return text; }
    bool SetText(std::string newText);


    virtual Vector2i GetCollisionCenter(void) const override { return center; }
    virtual Vector2i GetCollisionDimensions(void) const override;

    virtual void MoveElement(Vector2i moveAmount) override { center += moveAmount; }
    virtual void SetPosition(Vector2i newPos) override { center = newPos; }

    virtual void ScaleBy(Vector2f scaleAmount) override { Scale.MultiplyComponents(scaleAmount); }


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


private:


    Vector2i center, dimensions;
    std::string text;
};