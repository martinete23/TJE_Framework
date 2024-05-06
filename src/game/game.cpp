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

//for jump
bool is_jumping = false;
float jump_duration = 1.0f; // Duration of the jump animation in seconds
float jump_height = 250.0f; // Height of the jump
float initial_y_position = 0.0f; // Initial Y position before jumping
float jump_timer = 0.0f; // Timer for tracking the progress of the jump animation
bool free_camera = false;

//for movement
float camera_yaw = 0.f;
float camera_pitch = 0.f;
float camera_speed = 2.0f;


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

	// OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	// Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,100.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective

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
	camera->enable();

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
	drawGrid();

	current_stage->render();

	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	// Swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	//current_stage->update(seconds_elapsed);
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant
	if (free_camera) {

		// Example
		angle += (float)seconds_elapsed * 10.0f;

		// Mouse input to rotate the cam
		if (Input::isMousePressed(SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
		}

		// Async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);

		if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) free_camera = false;
	}

	else {
		//move the camera
		camera_yaw -= Input::mouse_delta.x * seconds_elapsed * (mouse_speed * 0.025f);
		camera_pitch -= Input::mouse_delta.y * seconds_elapsed * (mouse_speed * 0.025f);

		//pitch angle
		camera_pitch = clamp(camera_pitch, -M_PI * 0.4f, M_PI * 0.4f);

		//mYaw and mPitch
		Matrix44 mYaw;
		mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));
		Matrix44 mPitch;
		mPitch.setRotation(camera_pitch, Vector3(-1, 0, 0));

		//front
		Vector3 front = (mPitch * mYaw).frontVector().normalize();
		Vector3 eye;
		Vector3 center;

		//3d person camera position
		float orbit_dist = 1.0f;
		eye = World::instance->player->playerMatrix.getTranslation() - front * orbit_dist + Vector3(0.f, 0.5f, 0.f);
		center = World::instance->player->playerMatrix.getTranslation() + Vector3(0.f, 0.5f, 0.f);

		//set de camera
		camera->lookAt(eye, center, Vector3(0, 1, 0));

		//move the character
		Vector3 player_pos = World::instance->player->playerMatrix.getTranslation();

		Vector3 move_dir;
		Vector3 character_front = mYaw.frontVector();
		Vector3 character_right = mYaw.rightVector();
		// Async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift

		if (Input::isKeyPressed(SDL_SCANCODE_UP)) move_dir += character_front;
		if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) move_dir -= character_front;
		if (Input::isKeyPressed(SDL_SCANCODE_LEFT)) move_dir += character_right;
		if (Input::isKeyPressed(SDL_SCANCODE_RIGHT)) move_dir -= character_right;

		move_dir.normalize();
		move_dir *= 20.0f;
		player_pos += move_dir * seconds_elapsed;
		World::instance->player->playerMatrix.setTranslation(player_pos);
		World::instance->player->playerMatrix.rotate(camera_yaw, Vector3(0, 1, 0));


		if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) free_camera = true;

		if (Input::wasKeyPressed(SDL_SCANCODE_Z) && !is_jumping)
		{
			// Start the jump animation
			is_jumping = true;
			initial_y_position = World::instance->player->playerMatrix.m[13]; // Record the initial Y position
			jump_timer = 0.0f; // Reset the jump timer
		}
		if (is_jumping)
		{
			// Increment the jump timer
			jump_timer += (float)seconds_elapsed;

			// Calculate the progress of the jump animation (0 to 1)
			float jump_progress = jump_timer / jump_duration;

			// Calculate the new Y position using a sinusoidal function for smooth motion
			float new_y_position = initial_y_position + jump_height * sin(jump_progress * PI);

			// Update the Y position of the shape
			World::instance->player->playerMatrix.m[13] = new_y_position;

			// Check if the jump animation is complete
			if (jump_timer >= jump_duration)
			{
				// Reset the jump animation
				is_jumping = false;
				jump_timer = 0.0f;

				// Reset the Y position to its initial state
				World::instance->player->playerMatrix.m[13] = initial_y_position;
			}
		}
		//update the camera and movement

		//update movement



	}

}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
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
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

void Game::goToStage(eStages stage)
{
	current_stage = stages[stage];
	current_stage->onEnter();
}

