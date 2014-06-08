#pragma once

#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "../../Math/Vectors.h"
#include "../../Math/Array2D.h"

struct ManbilTexture;


//Wraps all data about a font's size.
struct FontSizeData
{
public:
    //At least one of the fields "CharWidth" and "CharHeight" must not be 0.
    //A value of 0 indicates that the value will match the other field's value.
    signed long CharWidth, CharHeight;
    //If both "HorizontalDPI" and "VerticalDPI" are 0, they will both default to 72.
    //Otherwise, a value of 0 indicates that the value will match the other field's value.
    unsigned int HorizontalDPI, VerticalDPI;

    FontSizeData(signed long charWidth = 0, signed long charHeight = 0, unsigned int horizontalDPI = 0, unsigned int verticalDPI = 0)
        : CharWidth(charWidth), CharHeight(charHeight), HorizontalDPI(horizontalDPI), VerticalDPI(verticalDPI)
    {

    }
};


//Wraps usage of the FreeType library.
//This instance's lifetime is coupled with the initialization and destruction of the Freetype system.
//Generally, any method that returns a boolean is returning whether or not that function succeeded.
//If a function fails, an error message will be exposed via GetError().
//This class is a singleton, and it should be checked for an error message before it is used for the first time.
class FreeTypeHandler
{
public:

    static const unsigned int ERROR_ID = 0;

    static FreeTypeHandler Instance;


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }


    //Returns the ID of the loaded font. If font creation failed, returns ERROR_ID.
    //A value of 0 for "characterWidth" means it is the same size as "characterHeight",
    //   and vice-versa. They cannot both be 0.
    //Same goes for horizontal-/verticalResolution; additionally, if horizontal- AND verticalResolution
    //   are 0, they default to 72.
    unsigned int LoadFont(std::string path, FontSizeData dat, signed long faceIndex = 0);

    //Loads the texture for the given font and character.
    bool LoadGlyph(unsigned int id, unsigned int charCode);
    //Gets a bitmap image for the most recently-loaded glyph for the given font.
    //Returns 0 if the given font doesn't exist.
    const FT_Bitmap * GetGlyph(unsigned int id) const
    {
        FaceMapLoc loc;
        if (!TryFindID(id, loc)) return 0;

        return &loc->second->glyph->bitmap;
    }

    //Attempts to set the font size for the given font.
    bool SetFontSize(unsigned int id, FontSizeData dat);
    //Attempts to directly set the pixel size for the given font's glyphs.
    bool SetFontSize(unsigned int id, unsigned int pixelWidth = 0, unsigned int pixelHeight = 0);

    //Gets the number of available glyphs for the given font face.
    //Returns 0 if the given id doesn't correspond to a font.
    unsigned int GetNumbGlyphs(unsigned int id) const;
    //Gets whether the given font can be scaled.
    //Returns false if the given font doesn't exist.
    bool GetCanBeScaled(unsigned int id) const;


    struct SupportedSizes { public: FT_Bitmap_Size * Sizes; unsigned int NumbSizes; };
    SupportedSizes GetSupportedSizes(unsigned int id);

    //Gets the width/height for the currently-loaded glyph for the given font.
    //Returns a width/height of 0 if the given font doesn't exist.
    Vector2i GetGlyphSize(unsigned int id) const;
    //Gets the amount to move to draw the next character.
    //Returns { 0, 0 } if the given font doesn't exist.
    Vector2i GetMoveToNextGlyph(unsigned int id) const;


    //Renders the given character into a private color array that can be accessed through "GetChar()".
    bool RenderChar(unsigned int fontID, unsigned int charToRender);

    //Gets the most recently-rendered char as a color array.
    const Array2D<Vector4b> & GetChar(void) const { return renderedText; }
    //Gets the most recently-rendered char and stores it into the given texture.
    bool GetChar(ManbilTexture & outTex) const;


private:

    unsigned int RoundUpToPowerOfTwo(unsigned int x);


    typedef std::unordered_map<unsigned int, FT_Face>::const_iterator FaceMapLoc;
    std::unordered_map<unsigned int, FT_Face> faces;

    unsigned int nextID;
    bool TryFindID(unsigned int id, FaceMapLoc & outLoc) const;

    FT_Library ftLib;

    Array2D<Vector4b> renderedText;

    mutable std::string errorMsg;


    //Singleton.
    FreeTypeHandler(void);
    ~FreeTypeHandler(void);
};