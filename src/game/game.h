/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#pragma once

#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/stage.h"

enum eSageLevel {
	TUTORIAL = 0,
	NEXUS = 1,
	LEVEL1 = 2,
	LEVEL2 = 3,
	LEVEL3 = 4,
	STOCK = 5,
	CHALLENGE = 6,
	TOTAL_STAGES = 7
};

struct CourseCrystals {
	bool FinalCrystal;
	bool FinalRedCrystal;
};

class Game
{
public:
	static Game* instance;

	World* world = nullptr;

	eSageLevel course = STOCK;

	Stage* stages[STAGES_SIZE];

	CourseCrystals CrystalTracking[TOTAL_STAGES];
	int CrystalCounter;

	bool displayImage = false;
	float timer = 0.0f;
	bool displayImage2 = false;
	float timer2 = 0.0f;
	bool displayImage3 = false;
	float timer3 = 0.0f;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	//some vars
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

	void goToStage(eStages stage);
};