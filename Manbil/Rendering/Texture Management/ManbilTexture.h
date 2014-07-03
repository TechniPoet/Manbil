#pragma once

#include "../../TextureSettings.h"
#include "TextureConverters.h"

/*
//Wraps all functionality for a texture.
struct ManbilTexture1
{
public:


    //Creates an instance with no texture yet.
    ManbilTexture1(void) : glTex(0) { }
    //Creates an instance with the given texture. May or may not be responsible for deleting the given texture.
    ManbilTexture1(RenderObjHandle texture, TextureSettings settings, unsigned int width, unsigned int height, bool responsibleForDeletion)
        : glTex(texture), settings(settings), responsibleForMemory(responsibleForDeletion), texWidth(width), texHeight(height)
    {
    
    }
    //Creates another reference to the same texture.
    //IMPORTANT NOTE: the copied instance isn't responsible for texture deletion!
    //IMPORTANT NOTE: changing the width/height/settings of any instance will not carry over into any other copy instance!
    ManbilTexture1(const ManbilTexture1 & cpy)
        : glTex(cpy.glTex), settings(cpy.settings), responsibleForMemory(false), texWidth(cpy.texWidth), texHeight(cpy.texHeight)
    {

    }

    ~ManbilTexture1(void)
    {
        Delete();
    }


    //Removes texture data from this texture and puts it into the given one.
    //Deletes the old texture data in "newHolder" if it exists.
    //This operation does not involve anything potentially expensive except
    //    for the deletion of the old OpenGL texture held in "newHolder".
    void TransferTo(ManbilTexture1 & newHolder)
    {
        newHolder.Delete();

        newHolder.glTex = glTex;
        newHolder.settings = settings;
        newHolder.texWidth = texWidth;
        newHolder.texHeight = texHeight;

        glTex = 0;
    }

    void Create(void)
    {
        if (glTex != 0) RenderDataHandler::DeleteTexture2D(glTex);
    }
    void Create(unsigned int width, unsigned int height)
    {
        if (glTex != 0) RenderDataHandler::DeleteTexture2D(glTex);
        RenderDataHandler::CreateTexture2D(glTex, Vector2i((int)width, (int)height));
        texWidth = width;
        texHeight = height;
    }

    //Deletes the current texture if this instance is supposed to.
    //Sets the texture being pointed at to 0.
    //Returns whether the texture was deleted.
    bool Delete(void)
    {
        if (responsibleForMemory && glTex != 0)
        {
            RenderDataHandler::DeleteTexture2D(glTex);
            glTex = 0;
            return true;
        }

        glTex = 0;
        return false;
    }


    void SetData(const Array2D<Vector4b> & inColor)
    {
        texWidth = inColor.GetWidth();
        texHeight = inColor.GetHeight();
        TextureConverters::ToTexture(inColor, glTex);
    }
    void SetData(const Array2D<Vector4f> & inColor)
    {
        texWidth = inColor.GetWidth();
        texHeight = inColor.GetHeight();
        RenderDataHandler::SetTexture2DDataFloats(glTex, Vector2i((int)texWidth, (int)texHeight), (void*)inColor.GetArray());
    }
    void SetData(const Vector4b color, unsigned int width, unsigned int height)
    {
        texWidth = width;
        texHeight = height;
        RenderDataHandler::SetTexture2DDataColor(glTex, Vector2i((int)texWidth, (int)texHeight), color);
    }
    void SetData(const Vector4f color, unsigned int width, unsigned int height)
    {
        texWidth = width;
        texHeight = height;
        RenderDataHandler::SetTexture2DDataColor(glTex, Vector2i((int)texWidth, (int)texHeight), color);
    }


    void SetWrapping(TextureSettings::TextureWrapping wrap) { SetWrapping(wrap, wrap); }
    void SetWrapping(TextureSettings::TextureWrapping horz, TextureSettings::TextureWrapping vert)
    {
        settings.HorWrap = horz;
        settings.VertWrap = vert;
        settings.SetWrappingData(glTex);
    }

    void SetFiltering(TextureSettings::TextureFiltering filter) { SetFiltering(filter, filter); }
    void SetFiltering(TextureSettings::TextureFiltering min, TextureSettings::TextureFiltering mag)
    {
        settings.MinFilter = min;
        settings.MagFilter = mag;
        settings.SetFilteringData(glTex);
    }

    void SetMipmapping(bool useMipmapping)
    {
        settings.GenerateMipmaps = useMipmapping;
        if (useMipmapping) RenderDataHandler::GenerateTexture2DMipmaps(glTex);
    }

    void Bind(void) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); }

    bool IsValid(void) const { return (glTex != 0); }


    //Gets whether this instance is responsible for deletion of the GL texture when the deconstructor is called.
    bool GetIsResponsibleForDeletion(void) const { return responsibleForMemory; }

    const TextureSettings & GetSettings(void) const { return settings; }
    unsigned int GetWidth(void) const { return texWidth; }
    unsigned int GetHeight(void) const { return texHeight; }

    
private:

    RenderObjHandle glTex;
    TextureSettings settings;
    unsigned int texWidth, texHeight;

    bool responsibleForMemory;
};
*/


















/*
//A texture value that can be either an OpenGL texture or an SFML texture.
//If both the "SFML texture" and "OpenGL texture" values are set,
//    the value that takes priority is the OpenGL texture.
struct ManbilTexture
{
public:

    RenderObjHandle GLTex;
    sf::Texture * SFMLTex;

    //Creates an invalid ManbilTexture (i.e. doesn't have a texture).
    ManbilTexture(void) : SFMLTex(0), GLTex(0) { }
    ManbilTexture(RenderObjHandle glTex) : SFMLTex(0), GLTex(glTex) { }
    ManbilTexture(sf::Texture * sfmlTex) : SFMLTex(sfmlTex), GLTex(0) { }

    void SetData(RenderObjHandle glTex) { SFMLTex = 0; GLTex = glTex; }
    void SetData(sf::Texture * sfmlTex) { SFMLTex = sfmlTex; GLTex = 0; }

    //Returns whether or not either texture type is set to a valid texture.
    bool IsValidTexture(void) const { return SFMLTex != 0 || GLTex != 0; }

    bool UsesGLTex(void) const { return GLTex != 0; }
    bool UsesSFMLTex(void) const { return !UsesGLTex() && SFMLTex != 0; }


    //Binds this texture. Returns whether or not this texture is valid.
    bool BindTexture(void) const
    {
        if (GLTex != 0) RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, GLTex);
        else if (SFMLTex != 0) sf::Texture::bind(SFMLTex);
        else return false;
        return true;
    }

    //Deletes the texture this instance holds
    //   (if it happens to hold both an SFML tex and a GL tex,
    //    it will delete both of them).
    //Returns false if neither texture type is set to a valid texture.
    bool DeleteTexture(void);


    //Returns false if neither texture type is set to a valid texture.
    bool SetData(const TextureSettings & settings) const
    {
        if (GLTex != 0)
        {
            settings.SetData(GLTex);
            return true;
        }
        else if (SFMLTex != 0)
        {
            settings.SetData(SFMLTex);
            return true;
        }

        return false;
    }

    //Returns false if neither texture type is set to a valid texture.
    bool SetWrapping(TextureSettings::TextureWrapping wrap) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetWrappingData(wrap);
        return true;
    }
    //Returns false if neither texture type is set to a valid texture.
    bool SetWrapping(TextureSettings::TextureWrapping horizontal, TextureSettings::TextureWrapping vertical) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetWrappingData(horizontal, vertical);
        return true;
    }

    //Returns false if neither texture type is set to a valid texture.
    bool SetFiltering(TextureSettings::TextureFiltering filter, bool usesMipmaps) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetFilteringData(filter, usesMipmaps);
        return true;
    }
    //Returns false if neither texture type is set to a valid texture.
    bool SetFiltering(TextureSettings::TextureFiltering minFilter, TextureSettings::TextureFiltering magFilter, bool usesMipmaps) const
    {
        if (!BindTexture()) return false;
        TextureSettings::SetFilteringData(minFilter, magFilter, usesMipmaps);
        return true;
    }

    //Returns false if neither texture type is set to a valid texture.
    bool GenerateMipmaps(void) const
    {
        if (!BindTexture()) return false;
        TextureSettings::GenMipmaps();
        return true;
    }

    //Returns { -1, -1 } if neither texture type is a valid texture.
    Vector2i GetSize(void) const
    {
        if (UsesSFMLTex())
            return Vector2i((int)SFMLTex->getSize().x, (int)SFMLTex->getSize().y);
        else if (UsesGLTex())
            return RenderDataHandler::GetTextureDimensions(GLTex);
        else return Vector2i(-1, -1);
    }
};
*/