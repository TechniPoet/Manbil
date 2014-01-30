#pragma once

#include "sfmlworld.h"
#include "SFML/Graphics.hpp"

#include "Fake2DArray.h"

class NoiseTest : public SFMLWorld
{
public:

	NoiseTest(void) : SFMLWorld(500, 500) { renderedNoise = NULL; renderedNoiseTex = NULL; }
	virtual ~NoiseTest(void) { DeleteData(); }

protected:

	void InitializeWorld(void);

	void UpdateWorld(float elapsedSeconds);
	void RenderWorld(float elapsedSeconds);

	void OnWorldEnd(void) { DeleteData(); }

private:

	void DeleteData(void) { DeleteAndSetToNull(renderedNoiseTex); DeleteAndSetToNull(renderedNoise); }

	void ReGenerateNoise(bool newSeeds);
	void InterpretNoise(const Fake2DArray<float> & noise);
	
	sf::Texture * renderedNoiseTex;
	sf::Sprite * renderedNoise;
};