#pragma once

#include <SFML/Graphics.hpp>
#include "../../Math/LowerMath.hpp"

//Calculates a normal map for the given bumpmap.
class BumpmapToNormalmap
{
public:
    
    static void Convert(const sf::Image & bumpmap, Fake2DArray<Vector3f> & outNormals);
    static void Convert(const Fake2DArray<float> & heightmap, Fake2DArray<Vector3f> & outNormals);
};