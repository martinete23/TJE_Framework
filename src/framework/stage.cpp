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
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material icon_material;

	icon_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	icon_material.diffuse = Texture::Get("data/textures/icon0.tga");
	icon_material.color = Vector4(1, 1, 1, 1);

	icon0 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
		Vector2(90, 48), icon_material);

	icon_material.diffuse = Texture::Get("data/textures/icon1.tga");

	icon1 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
		Vector2(90, 48), icon_material);

	icon_material.diffuse = Texture::Get("data/textures/icon2.tga");

	icon2 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
		Vector2(90, 48), icon_material);

	icon_material.diffuse = Texture::Get("data/textures/icon3.tga");

	icon3 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
		Vector2(90, 48), icon_material);

	icon_material.diffuse = Texture::Get("data/textures/icon4.tga");

	icon4 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
		Vector2(90, 48), icon_material);

	icon_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	icon_material.diffuse = Texture::Get("data/textures/crystal_obtained.tga");
	icon_material.color = Vector4(1, 1, 1, 1);

	crystal_obtained_icon = new EntityUI(Vector2(Game::instance->window_width/2, 50),
		Vector2(600, 100), icon_material);

	texture_cube.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubetext.fs");
	texture_cube.diffuse = new Texture();
	if (Game::instance->course == TUTORIAL) {
		texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/cave_cubemap/px.png", "data/textures/cave_cubemap/nx.png", 
			"data/textures/cave_cubemap/ny.png", "data/textures/cave_cubemap/py.png", "data/textures/cave_cubemap/pz.png", "data/textures/cave_cubemap/nz.png" });
		channel = Audio::Play("data/sounds/Tutorial_Course.mp3", 0.3, BASS_SAMPLE_LOOP);
	}
	else if (Game::instance->course == NEXUS) {
		texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/BlackSky/px.png", "data/textures/BlackSky/nx.png",
			"data/textures/BlackSky/ny.png", "data/textures/BlackSky/py.png", "data/textures/BlackSky/pz.png", "data/textures/BlackSky/nz.png" });
		channel = Audio::Play("data/sounds/Nexus_Course.mp3", 0.3, BASS_SAMPLE_LOOP);
	}
	else if (Game::instance->course == LEVEL1) {
		texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/BlueSky/px.png", "data/textures/BlueSky/nx.png",
			"data/textures/BlueSky/ny.png", "data/textures/BlueSky/py.png", "data/textures/BlueSky/pz.png", "data/textures/BlueSky/nz.png" });
		channel = Audio::Play("data/sounds/theme.mp3", 0.3, BASS_SAMPLE_LOOP);
	}
	else if (Game::instance->course == LEVEL2) {
		texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/RedSky/px.png", "data/textures/RedSky/nx.png", 
			"data/textures/RedSky/ny.png", "data/textures/RedSky/py.png", "data/textures/RedSky/pz.png", "data/textures/RedSky/nz.png" });
		channel = Audio::Play("data/sounds/theme.mp3", 0.3, BASS_SAMPLE_LOOP);
	}
	

	skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.obj"), texture_cube, "cubemap");
	
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

	camera2D->enable();
	if (World::instance->showCrystalobtainedIcon) {
		crystal_obtained_icon->render(camera2D);
	}
	if (Game::instance->CrystalCounter == 0)
	{
		icon0->render(camera2D);
	}
	if (Game::instance->CrystalCounter == 1)
	{
		icon1->render(camera2D);
	}
	if (Game::instance->CrystalCounter == 2)
	{
		icon2->render(camera2D);
	}
	if (Game::instance->CrystalCounter == 3)
	{
		icon3->render(camera2D);
	}
	if (Game::instance->CrystalCounter == 4)
	{
		icon4->render(camera2D);
	}

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
		
		World::instance = new World();
	}
	loading_time += 1 * seconds_elapsed;

	if (loading_time > 1.0f) {

		Game::instance->goToStage(PLAY);
	}
}
