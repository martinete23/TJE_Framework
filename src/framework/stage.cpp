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
	material_background.diffuse = Texture::Get("data/textures/lletres.tga");
	material_background.color = Vector4(1, 1, 1, 1);

	background = new EntityUI(Vector2(Game::instance->window_width / 2, 140),
		Vector2(776, 280), material_background);

	Material material_button;

	material_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_button.diffuse = Texture::Get("data/textures/play_button.tga");
	material_button.color = Vector4(1, 1, 1, 1);

	playButton = new EntityUI(Vector2(Game::instance->window_width/2, Game::instance->window_height / 2 + Game::instance->window_height / 4), 
		Vector2(100, 40), material_button, BUTTONPLAY, "play_button");

	background->addChild(playButton);

	material_button.diffuse = Texture::Get("data/textures/quit_button.tga");

	quitButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 3),
		Vector2(100, 40), material_button, BUTTONQUIT, "quit_button");

	background->addChild(quitButton);

	material_button.diffuse = Texture::Get("data/textures/controls_button.tga");

	tutorialButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 2.4),
		Vector2(100, 40), material_button, BUTTONCONTROLLER, "controller_button");

	background->addChild(tutorialButton);

	texture_cube.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubetext.fs");
	texture_cube.diffuse = new Texture();
	texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/cave_cubemap/px.png", "data/textures/cave_cubemap/nx.png",
		"data/textures/cave_cubemap/ny.png", "data/textures/cave_cubemap/py.png", "data/textures/cave_cubemap/pz.png", "data/textures/cave_cubemap/nz.png" });
		
	skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.obj"), texture_cube, "cubemap");
	World::instance->player->animator.playAnimation("data/animations/twerk.skanim");
	channel = Audio::Play("data/sounds/Intro_game.mp3", 0.3, BASS_SAMPLE_LOOP);
}

void IntroStage::onExit()
{
	Audio::Stop(channel);
}

void IntroStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	World::instance->render();
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

	World::instance->camera->eye = Vector3(30 * cos(Game::instance->time * 0.05), 20, 30 * sin(Game::instance->time * 0.05));
	World::instance->player->animationUpdate(seconds_elapsed);
}

void PlayStage::onEnter()
{
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_portal;

	material_portal.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_portal.diffuse = Texture::Get("data/textures/level2.tga");
	material_portal.color = Vector4(1, 1, 1, 1);

	level2 = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 - 200), Vector2(400, 80), material_portal);

	material_portal.diffuse = Texture::Get("data/textures/level3.tga");

	level3 = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 - 200), Vector2(400, 80), material_portal);

	icon_redCrystal_manterial.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	icon_redCrystal_manterial.diffuse = Texture::Get("data/textures/icon_redCrystal.tga");
	icon_redCrystal_manterial.color = Vector4(1, 1, 1, 1);

	icon_RedCrystals = new EntityUI(Vector2(-120, 40),
		Vector2(240, 48), icon_redCrystal_manterial);

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

	icon_material.diffuse = Texture::Get("data/textures/icon5.tga");

	icon5 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
		Vector2(90, 48), icon_material);

	icon_material.diffuse = Texture::Get("data/textures/icon6.tga");

	icon6 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
		Vector2(90, 48), icon_material);

	icon_material.diffuse = Texture::Get("data/textures/icon7.tga");

	icon7 = new EntityUI(Vector2(Game::instance->window_width - 50, 35),
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
		channel = Audio::Play("data/sounds/Castle_Theme.mp3", 0.3, BASS_SAMPLE_LOOP);
	}
	else if (Game::instance->course == LEVEL2) {
		texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/RedSky/px.png", "data/textures/RedSky/nx.png", 
			"data/textures/RedSky/ny.png", "data/textures/RedSky/py.png", "data/textures/RedSky/pz.png", "data/textures/RedSky/nz.png" });
		channel = Audio::Play("data/sounds/Volcano_Theme.mp3", 0.3, BASS_SAMPLE_LOOP);
	}
	else if (Game::instance->course == LEVEL3) {
		texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/PurpleSky/px.png", "data/textures/PurpleSky/nx.png",
			"data/textures/PurpleSky/ny.png", "data/textures/PurpleSky/py.png", "data/textures/PurpleSky/pz.png", "data/textures/PurpleSky/nz.png" });
		channel = Audio::Play("data/sounds/Challenge_Course.mp3", 0.3, BASS_SAMPLE_LOOP);
	}

	skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.obj"), texture_cube, "cubemap");
	
	for (int i = 0; i < 20; ++i)
	{
		std::string texturePath = "data/textures/animation/frame" + std::to_string(i) + ".tga";
		Texture* frame = Texture::Get(texturePath.c_str());
		animationFrames.push_back(frame);
	}

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
	World::instance->render();
	glDisable(GL_DEPTH_TEST);
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
	if (Game::instance->CrystalCounter == 5)
	{
		icon5->render(camera2D);
	}
	if (Game::instance->CrystalCounter == 6)
	{
		icon6->render(camera2D);
	}
	if (Game::instance->CrystalCounter == 7)
	{
		icon7->render(camera2D);
	}

	icon_RedCrystals->render(camera2D);

	if (Game::instance->displayImage == true && Game::instance->timer <= 2.0f)
	{
		level2->render(camera2D);
	}
	else
	{
		Game::instance->displayImage = false;
	}

	if (Game::instance->displayImage2 == true && Game::instance->timer2 <= 2.0f)
	{
		level3->render(camera2D);
	}
	else
	{
		Game::instance->displayImage2 = false;
	}

	if (World::instance->Portal_Animated) {
		Material animationMaterial;
		animationMaterial.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		animationMaterial.diffuse = animationFrames[currentFrame];
		animationMaterial.color = Vector4(1, 1, 1, 1);

		EntityUI animationEntity(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2), animationMaterial);
		animationEntity.render(camera2D);
	}
}

void PlayStage::update(double seconds_elapsed)
{
	World::instance->update(seconds_elapsed);

	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		Game::instance->goToStage(PAUSE);
	}
	if (World::instance->crystalsCollected == 1) {
		icon_RedCrystals = new EntityUI(Vector2(-90, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}
	else if (World::instance->crystalsCollected == 2) {
		icon_RedCrystals = new EntityUI(Vector2(-60, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}
	else if (World::instance->crystalsCollected == 3) {
		icon_RedCrystals = new EntityUI(Vector2(-30, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}
	else if (World::instance->crystalsCollected == 4) {
		icon_RedCrystals = new EntityUI(Vector2(0, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}
	else if (World::instance->crystalsCollected == 5) {
		icon_RedCrystals = new EntityUI(Vector2(30, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}
	else if (World::instance->crystalsCollected == 6) {
		icon_RedCrystals = new EntityUI(Vector2(60, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}
	else if (World::instance->crystalsCollected == 7) {
		icon_RedCrystals = new EntityUI(Vector2(90, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}
	else if (World::instance->crystalsCollected == 8) {
		icon_RedCrystals = new EntityUI(Vector2(120, 40),
			Vector2(240, 48), icon_redCrystal_manterial);
	}

	if (Game::instance->displayImage == true)
	{
		Game::instance->timer += 1.0f * seconds_elapsed;
	}
	else
	{
		Game::instance->timer = 0.0f;
	}

	if (Game::instance->displayImage2 == true)
	{
		Game::instance->timer2 += 1.0f * seconds_elapsed;
	}
	else
	{
		Game::instance->timer2 = 0.0f;
	}
	if (World::instance->Portal_Animated) {
		timeSinceLastFrame += seconds_elapsed;
		if (timeSinceLastFrame >= 1.0f / animationSpeed)
		{
			currentFrame = (currentFrame + 1) % animationFrames.size();
			timeSinceLastFrame = 0.0f;
		}
	}

}

void WinStage::onEnter()
{
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_background;

	material_background.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_background.diffuse = Texture::Get("data/textures/The_End.tga");
	material_background.color = Vector4(1, 1, 1, 1);

	background = new EntityUI(Vector2(776, 280), material_background);

	Material material_play_button;

	material_play_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_play_button.diffuse = Texture::Get("data/textures/play_button.tga");
	material_play_button.color = Vector4(1, 1, 1, 1);

	playButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 4),
		Vector2(100, 40), material_play_button, BUTTONPLAY, "play_button");

	background->addChild(playButton);

	Material material_quit_button;

	material_quit_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_quit_button.diffuse = Texture::Get("data/textures/quit_button.tga");
	material_quit_button.color = Vector4(1, 1, 1, 1);

	quitButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 3),
		Vector2(100, 40), material_quit_button, BUTTONQUIT, "quit_button");

	background->addChild(quitButton);

	texture_cube.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubetext.fs");
	texture_cube.diffuse = new Texture();
	texture_cube.diffuse->loadCubemap("cubemap", { "data/textures/cave_cubemap/px.png", "data/textures/cave_cubemap/nx.png",
		"data/textures/cave_cubemap/ny.png", "data/textures/cave_cubemap/py.png", "data/textures/cave_cubemap/pz.png", "data/textures/cave_cubemap/nz.png" });

	skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.obj"), texture_cube, "cubemap");
	World::instance->player->animator.playAnimation("data/animations/twerk.skanim");
	channel = Audio::Play("data/sounds/Final_Course.mp3", 0.3);
}

void WinStage::onExit()
{
	Audio::Stop(channel);
}

void WinStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	World::instance->render();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera2D->enable();

	background->render(camera2D);
}

void WinStage::update(double seconds_elapsed)
{
	background->update(seconds_elapsed);
	Vector3 CameraPos = World::instance->player->playerMatrix.getTranslation();
	World::instance->camera->eye = Vector3(CameraPos.x - 2 * cos(Game::instance->time * 0.05), 3, CameraPos.z - 2 * sin(Game::instance->time * 0.05));
	World::instance->camera->center = Vector3(CameraPos.x, CameraPos.y, CameraPos.z);
	World::instance->player->animationUpdate(seconds_elapsed);
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
		Vector2(160, 40), material_play_again_button, PLAYAGAIN, "play_again_button");

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
		if (Game::instance->course == STOCK) {
			Game::instance->goToStage(WIN);
		}
		else {
			Game::instance->goToStage(PLAY);
		}
	}
}

void PauseStage::onEnter()
{
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_background;

	material_background.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_background.diffuse = Texture::Get("data/textures/pausa.tga");
	material_background.color = Vector4(1, 1, 1, 1);

	background = new EntityUI(Vector2(800, 600), material_background);

	Material material_play_button;

	material_play_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_play_button.diffuse = Texture::Get("data/textures/continue_button.tga");
	material_play_button.color = Vector4(1, 1, 1, 1);

	playButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2),
		Vector2(100, 40), material_play_button, CONTINUE, "continue_button");

	background->addChild(playButton);

	Material material_exitCourse_button;
	material_exitCourse_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_exitCourse_button.diffuse = Texture::Get("data/textures/nexus_button.tga");
	material_exitCourse_button.color = Vector4(1, 1, 1, 1);

	exitCourseButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 10),
		Vector2(100, 40), material_exitCourse_button, BUTTONEXITCOURSE, "nexus_button");

	background->addChild(exitCourseButton);

	Material material_quit_button;

	material_quit_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_quit_button.diffuse = Texture::Get("data/textures/quit_button.tga");
	material_quit_button.color = Vector4(1, 1, 1, 1);

	quitButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2 + Game::instance->window_height / 5),
		Vector2(100, 40), material_quit_button, BUTTONQUIT, "quit_button");

	background->addChild(quitButton);


}

void PauseStage::onExit()
{
}

void PauseStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera2D->enable();

	background->render(camera2D);
}

void PauseStage::update(double seconds_elapsed)
{
	background->update(seconds_elapsed);
}

void TutorialStage::onEnter()
{
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_background;

	material_background.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_background.diffuse = Texture::Get("data/textures/controls1.tga");
	material_background.color = Vector4(1, 1, 1, 1);

	background = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 2),
		Vector2(800, 600), material_background);

	Material button_material;

	button_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	button_material.diffuse = Texture::Get("data/textures/press_space_button.tga");
	button_material.color = Vector4(1, 1, 1, 1);

	nextButton = new EntityUI(Vector2(Game::instance->window_width / 2 + Game::instance->window_width / 4, Game::instance->window_height - 50),
		Vector2(100, 40), button_material, BUTTONNEXT, "button_back");

	background->addChild(nextButton);
	channel = Audio::Play("data/sounds/controller_screen.mp3", 0.3, BASS_SAMPLE_LOOP);
}

void TutorialStage::onExit()
{
	Audio::Stop(channel);
}

void TutorialStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera2D->enable();

	if (Controller_background == 1) {
		background->material.diffuse = Texture::Get("data/textures/controls2.tga");
	}
	else if (Controller_background == 2) {
		background->material.diffuse = Texture::Get("data/textures/controls3.tga");
	}
	else if (Controller_background == 3) {
		background->material.diffuse = Texture::Get("data/textures/controls4.tga");
	}
	
	background->render(camera2D);
}

void TutorialStage::update(double seconds_elapsed)
{
	background->update(seconds_elapsed);

	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
		if (Controller_background < 3) {
			Controller_background += 1;
		}
		else {
			Controller_background = 0;
			Game::instance->goToStage(INTRO);
		}
	}
}

void CoursesSelectStage::onEnter()
{
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1.0f, 1.0f);

	Material material_background;

	material_background.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_background.diffuse = Texture::Get("data/textures/select_stage_background.tga");
	material_background.color = Vector4(1, 1, 1, 1);

	background = new EntityUI(Vector2(800, 600), material_background);

	Material material_button;

	material_button.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	material_button.diffuse = Texture::Get("data/textures/tutorialCourse_button.tga");
	material_button.color = Vector4(1, 1, 1, 1);

	TutorialButton = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 3),
		Vector2(118, 40), material_button, BUTTONTUTORIAL, "tutorial_button");

	background->addChild(TutorialButton);

	material_button.diffuse = Texture::Get("data/textures/level1_button.tga");

	Level1Button = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 3 + 80),
		Vector2(118, 40), material_button, BUTTONLEVEL1, "level1_button");

	background->addChild(Level1Button);

	material_button.diffuse = Texture::Get("data/textures/level2_button.tga");

	Level2Button = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 3 + 160),
		Vector2(118, 40), material_button, BUTTONLEVEL2, "level2_button");

	background->addChild(Level2Button);

	material_button.diffuse = Texture::Get("data/textures/level3_button.tga");

	Level3Button = new EntityUI(Vector2(Game::instance->window_width / 2, Game::instance->window_height / 3 + 240),
		Vector2(118, 40), material_button, BUTTONLEVEL3, "level3_button");

	background->addChild(Level3Button);

}

void CoursesSelectStage::onExit()
{
}

void CoursesSelectStage::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera2D->enable();

	background->render(camera2D);
}

void CoursesSelectStage::update(double seconds_elapsed)
{
	background->update(seconds_elapsed);
}