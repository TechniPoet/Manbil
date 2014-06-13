#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Input/InputManager.h"
#include "Rendering/Texture Management/TextureManager.h"
#include "Events/Timing.h"



//A basic world using SFML. Also initializes the SFGUI library upon creation.
class SFMLWorld
{
public:

	SFMLWorld(int windWidth, int windHeight, sf::ContextSettings settings = sf::ContextSettings())
        : contextSettings(settings), totalElapsedSeconds(0.0f), window(0), windowWidth(windWidth), windowHeight(windHeight)
    {
        window = 0;
    }
	virtual ~SFMLWorld(void) { if (window) delete window; }

	float GetTotalElapsedSeconds(void) const { return totalElapsedSeconds; }
	sf::RenderWindow * GetWindow(void) const { return window; }

    const sf::ContextSettings & GetContextSettings(void) const { return contextSettings; }

	bool IsGameOver(void) const { return worldOver; }
	
	void EndWorld(void) { worldOver = true; }

	//Starts running this world in an endless loop until it's finished.
	void RunWorld(void);

    InputManager<unsigned int> Input;
    TimerManager Timers;
    TextureManager Textures;
    //TODO: Move the TextureManager into SFMLOpenGLWorld, because it uses OpenGL stuff.

protected:

#pragma warning(disable: 4100)

	template<class SomeClass>
	void DeleteAndSetToNull(SomeClass *& pointer) { if (pointer) delete pointer; pointer = 0; }

	virtual void InitializeWorld(void) { }

	virtual void UpdateWorld(float elapsedSeconds) = 0;
	virtual void RenderWorld(float elapsedSeconds) = 0;

	virtual void OnCloseWindow(void) { worldOver = true; }
	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) { }
	virtual void OnOtherWindowEvent(sf::Event & windowEvent) { }

	virtual void OnWorldEnd(void) { }

#pragma warning(default: 4100)

private:

	float totalElapsedSeconds;
	sf::RenderWindow * window;
	int windowWidth, windowHeight;

    sf::ContextSettings contextSettings;

	bool worldOver;
};