#include "stage.h"
#include "game/game.h"
#include "framework/input.h"
#include "entities/entitymesh.h"

void IntroStage::onEnter()
{
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_background;

	material_background.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_background.diffuse = Texture::Get("data/textures/background.tga");
	material_background.color = Vector4(1, 1, 1, 1);

	background = new EntityUI(Vector2(800, 600), material_background);

	Material material_play_button;

	material_play_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_play_button.diffuse = Texture::Get("data/textures/play_button.tga");
	material_play_button.color = Vector4(1, 1, 1, 1);

	playButton = new EntityUI(Vector2(Game::instance->window_width/2, Game::instance->window_height/2 + Game::instance->window_height / 4), 
		Vector2(80, 40), material_play_button, BUTTONPLAY, "play_button");

	background->addChild(playButton);

	Material material_quit_button;

	material_quit_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_quit_button.diffuse = Texture::Get("data/textures/quit_button.tga");
	material_quit_button.color = Vector4(1, 1, 1, 1);

	quitButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 3),
		Vector2(80, 40), material_quit_button, BUTTONQUIT, "quit_button");

	background->addChild(quitButton);
}

void IntroStage::onExit()
{
}

void IntroStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera2D->enable();

	background->render(camera2D);
}

void IntroStage::update(double seconds_elapsed)
{

	background->update(seconds_elapsed);
	
}

void PlayStage::onEnter()
{
	texture_cube = Texture::Get("data/textures/StandardCubeMap.tga");

	mesh_cube = Mesh::Get("data/meshes/cubemap.obj");

	shader_cube = Shader::Get("data/shaders/basic.vs", "data/shaders/cubetext.fs");

}

void PlayStage::onExit()
{
}

void PlayStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Ensure we clear the buffers
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	World::instance->render();

	if (shader_cube)
	{
		// Enable shader
		shader_cube->enable();

		// Upload uniforms
		shader_cube->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader_cube->setUniform("u_viewprojection", World::instance->camera->viewprojection_matrix);
		shader_cube->setUniform("u_texture", texture_cube, 0);
		shader_cube->setUniform("u_model", m_cube);
		shader_cube->setUniform("u_time", time);


		// Do the draw call
		mesh_cube->render(GL_TRIANGLES);

		// Disable shader
		shader_cube->disable();
	}
}

void PlayStage::update(double seconds_elapsed)
{
	World::instance->update(seconds_elapsed);
}

void WinStage::onEnter()
{
}

void WinStage::onExit()
{
}

void WinStage::render()
{
}

void WinStage::update(double seconds_elapsed)
{
}

void LoseStage::onEnter()
{
}

void LoseStage::onExit()
{
}

void LoseStage::render()
{
}

void LoseStage::update(double seconds_elapsed)
{
}

void LoadingStage::onEnter()
{
}

void LoadingStage::onExit()
{
}

void LoadingStage::render()
{
}

void LoadingStage::update(double seconds_elapsed)
{
}
