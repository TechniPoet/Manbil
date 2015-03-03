#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "../Input/InputManager.h"
#include "../Events/Timing.h"



//A basic world using SFML. Also initializes the SFGUI library upon creation.
class SFMLWorld
{
public:

	SFMLWorld(int windWidth, int windHeight,
              sf::ContextSettings settings = sf::ContextSettings());
	virtual ~SFMLWorld(void);

	float GetTotalElapsedSeconds(void) const { return totalElapsedSeconds; }
	sf::RenderWindow* GetWindow(void) const { return window; }

    const sf::ContextSettings& GetContextSettings(void) const { return contextSettings; }

	bool IsGameOver(void) const { return worldOver; }
	
	void EndWorld(void) { worldOver = true; }

	//Starts running this world in an endless loop until it's finished.
	void RunWorld(void);

    InputManager<unsigned int> Input;
    TimerManager Timers;


protected:

#pragma warning(disable: 4100)

    //Gets the video mode for this game window when it's initialized.
    //Default behavior: creates a windowed mode with the given width/height and a bit depth of 32.
    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH)
    {
        return sf::VideoMode(windowW, windowH, 32);
    }
    //Gets the title bar of the game window.
    //Default behavior: returns "World window".
    virtual std::string GetWindowTitle(void)
    {
        return "World window";
    }
    //Gets the window style for this game window when it's initialized.
    //Default behavior: returns sf::Style::Default.
    virtual sf::Uint32 GetSFStyleFlags(void)
    {
        return sf::Style::Default;
    }
    //TODO: Make another function for the sf::Context instead of taking it via the constructor.


	virtual void InitializeWorld(void) { }

	virtual void UpdateWorld(float elapsedSeconds) = 0;
	virtual void RenderWorld(float elapsedSeconds) = 0;

	virtual void OnCloseWindow(void) { worldOver = true; }
	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) { }
	virtual void OnOtherWindowEvent(sf::Event& windowEvent) { }

	virtual void OnWorldEnd(void) { }

#pragma warning(default: 4100)


private:

	float totalElapsedSeconds;
	sf::RenderWindow* window;
	int windowWidth, windowHeight;

    sf::ContextSettings contextSettings;

	bool worldOver;
};