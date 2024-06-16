#include "game.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"

#include <cmath>

//some globals
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
float angle = 0;
float mouse_speed = 100.0f;

Game* Game::instance = NULL;



Stage* current_stage;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	world = new World();

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	CrystalCounter = 0;
	for (int i = 0; i < TOTAL_STAGES; i++) {
		CrystalTracking[i].FinalCrystal = false;
		CrystalTracking[i].FinalRedCrystal = false;
	}

	// OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	// Create our camera

	//// Load one texture using the Texture Manager
	//texture = Texture::Get("data/textures/texture.tga");

	//// Example of loading Mesh from Mesh Manager
	//mesh = Mesh::Get("data/meshes/box.ASE");

	//// Example of shader loading using the shaders manager
	//shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	// Hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	stages[INTRO] = new IntroStage();
	stages[PLAY] = new PlayStage();
	stages[WIN] = new WinStage();
	stages[LOSE] = new LoseStage();
	stages[LOADING] = new LoadingStage();
	stages[PAUSE] = new PauseStage();
	stages[TUTORIALIMAGE] = new TutorialStage();
	stages[COURSESELECT] = new CoursesSelectStage();
	goToStage(INTRO);
}

//what to do when the image has to be draw
void Game::render(void)
{
	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
   
	//// Create model matrix for cube
	//Matrix44 m;
	//m.rotate(angle*DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));

	//if(shader)
	//{
	//	// Enable shader
	//	shader->enable();

	//	// Upload uniforms
	//	shader->setUniform("u_color", Vector4(1,1,1,1));
	//	shader->setUniform("u_viewprojection", camera->viewprojection_matrix );
	//	shader->setUniform("u_texture", texture, 0);
	//	shader->setUniform("u_model", m);
	//	shader->setUniform("u_time", time);

	//	// Do the draw call
	//	mesh->render( GL_TRIANGLES );

	//	// Disable shader
	//	shader->disable();
	//}

	// Draw the floor grid


	current_stage->render();

	//drawGrid();

	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	// Swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	current_stage->update(seconds_elapsed);

	if (Input::isKeyPressed(SDL_SCANCODE_P)) {
		goToStage(COURSESELECT);
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));
	}

}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{

}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1f : 0.9f;
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	World::instance->camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
	current_stage->onExit();
	current_stage->onEnter();
}

void Game::goToStage(eStages stage)
{
	if (current_stage) {
		current_stage->onExit();
	}

	current_stage = stages[stage];

	if (current_stage) {
		std::cout << "Entering stage: " << stage << std::endl;
		current_stage->onEnter();
	}
	else {
		std::cerr << "Error: Stage not found" << std::endl;
	}
}

