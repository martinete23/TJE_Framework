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

	Audio::Play("data/sounds/start.wav", 0.5);
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
	texture_cube.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubetext.fs");
	texture_cube.diffuse = new Texture();
	texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/cave_cubemap/px.png", "data/textures/cave_cubemap/nx.png", "data/textures/cave_cubemap/ny.png", "data/textures/cave_cubemap/py.png", "data/textures/cave_cubemap/pz.png", "data/textures/cave_cubemap/nz.png" });

	skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.obj"), texture_cube, "cubemap");

	channel = Audio::Play("data/sounds/theme.mp3", 0.3, BASS_SAMPLE_LOOP);
}

void PlayStage::onExit()
{
	Audio::Stop(channel);
}

void PlayStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Ensure we clear the buffers
	glDisable(GL_DEPTH_TEST);
	if (texture_cube.shader)
	{
		texture_cube.shader->enable();
		texture_cube.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		texture_cube.shader->setUniform("u_viewprojection", World::instance->camera->viewprojection_matrix);
		texture_cube.shader->setUniform("u_texture", texture_cube.diffuse, 0);
		skybox->render(World::instance->camera);
		texture_cube.shader->disable();
	}
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	World::instance->render();
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
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_background;

	material_background.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_background.diffuse = Texture::Get("data/textures/lose.tga");
	material_background.color = Vector4(1, 1, 1, 1);

	background = new EntityUI(Vector2(800, 600), material_background);

	Material material_play_again_button;

	material_play_again_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_play_again_button.diffuse = Texture::Get("data/textures/play_again_button.tga");
	material_play_again_button.color = Vector4(1, 1, 1, 1);

	playAgainButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 4),
		Vector2(160, 40), material_play_again_button, BUTTONPLAY, "play_again_button");

	background->addChild(playAgainButton);

	Audio::Play("data/sounds/lose.wav", 0.5);
}

void LoseStage::onExit()
{
}

void LoseStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera2D->enable();

	background->render(camera2D);
}

void LoseStage::update(double seconds_elapsed)
{
	background->update(seconds_elapsed);
}

void LoadingStage::onEnter()
{
	loading_time = 0;
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_loading;

	material_loading.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_loading.diffuse = Texture::Get("data/textures/loading_screen.tga");
	material_loading.color = Vector4(1, 1, 1, 1);

	loading = new EntityUI(Vector2(800, 600), material_loading);


}

void LoadingStage::onExit()
{
}

void LoadingStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera2D->enable();

	loading->render(camera2D);
}

void LoadingStage::update(double seconds_elapsed)
{
	loading->update(seconds_elapsed);
	if (loading_time == 0) {
		Game::instance->course = LEVEL1;
		World::instance = new World();
	}
	loading_time += 1 * seconds_elapsed;

	if (loading_time > 1.0f) {
		Game::instance->goToStage(PLAY);
	}
}
