#include "world.h"
#include "entities/entitymesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include <game/game.h>

World* World::instance;

World::World()
{
	instance = this;

	int window_width = Game::instance->window_width;
	int window_height = Game::instance->window_height;

	root = new Entity();

	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	
	Material player_material;
	//player_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	player_material.shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	player_material.diffuse = Texture::Get("data/textures/polygon.tga");
	player_material.color = Vector4(1, 1, 1, 1);

	//player = new EntityPlayer(Mesh::Get("data/meshes/polygon.obj"), player_material);
	player = new EntityPlayer(Mesh::Get("data/meshes/polygon.MESH"), player_material);
	player->isAnimated = true;

	Material crystal_material;
	crystal_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	crystal_material.diffuse = Texture::Get("data/textures/red_cristal_texture.tga");
	crystal_material.color = Vector4(1, 1, 1, 1);
	crystalsCollected = 0;
	for (int i = 0; i < RED_CRISTALS_TOT; i++) {
		Redcrystals[i] = new EntityCrystal(Mesh::Get("data/meshes/crystal.obj"), crystal_material, "red_crystal");
	}

	crystal_material.diffuse = Texture::Get("data/textures/yellow_cristal_texture.tga");
	for (int i = 0; i < YELLOW_CRISTALS_TOT; i++) {
		Yellowcrystals[i] = new EntityCrystal(Mesh::Get("data/meshes/crystal.obj"), crystal_material, "yellow_crystal");
	}

	if (Game::instance->course == TUTORIAL) {
		parseScene("data/Tutorial.scene", root);
	}
	else if (Game::instance->course == NEXUS) {
		parseScene("data/NexusWorld.scene", root);
	}
	else if (Game::instance->course == LEVEL1) {
		parseScene("data/Level1.scene", root);
	}
	else if (Game::instance->course == LEVEL2) {
		parseScene("data/Level2.scene", root);
	}
}

void World::render()
{
	camera->enable();

	root->render(camera);

	player->render(camera);

	for (int i = 0; i < RED_CRISTALS_TOT; i++) {
		if (Redcrystals[i]->active) {
			Redcrystals[i]->render(camera);
		}
	}
	for (int i = 0; i < YELLOW_CRISTALS_TOT; i++) {
		if (Yellowcrystals[i]->active) {
			Yellowcrystals[i]->render(camera);
		}
	}
}

void World::update(float delta_time)
{

	root->update(delta_time);

	for (int i = 0; i < RED_CRISTALS_TOT; i++) {
		if (Redcrystals[i]->active) {
			Redcrystals[i]->update(delta_time);
		}
	}
	for (int i = 0; i < YELLOW_CRISTALS_TOT; i++) {
		if (Yellowcrystals[i]->active) {
			Yellowcrystals[i]->update(delta_time);
		}
	}
	if (CrystalAnimation) {
		animation_in_game(delta_time);
	}
	else if (YellowCrystalCollectedAnimation) {
		animation_get_crystal(delta_time);
	}
	else {
		player->update(delta_time);

		camera_yaw -= Input::mouse_delta.x * 0.005f;
		camera_pitch -= Input::mouse_delta.y * 0.005f;

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

		float orbit_dist = 1.5f;
		eye = World::instance->player->playerMatrix.getTranslation() - front * orbit_dist;
		center = World::instance->player->playerMatrix.getTranslation() + Vector3(0.f, 0.5f, 0.f);
		EntityCollider* ec = new EntityCollider;
		Vector3 dir = eye - center;
		sCollisionData data = ec->raycast(center, dir.normalize(), eCollisionFilter::ALL, dir.length());

		if (data.collided) {
			eye = center + dir.normalize() * data.distance;
		}
		camera->lookAt(eye, center, Vector3(0, 1, 0));

		delete ec;
	}

}

bool World::parseScene(const char* filename, Entity* root)
{
	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		if (mesh_name[0] == '#')
			continue;

		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}

	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;

		sRenderData& render_data = data.second;

		// No transforms, nothing to do here
		if (render_data.models.empty())
			continue;


		Material mat = render_data.material;
		EntityCollider* new_entity = nullptr;

		size_t tag = data.first.find("@tag");
		size_t playerTag = data.first.find("@player");
		size_t YellowCrystal = data.first.find("@Y");
		size_t YellowRed = data.first.find("@YR");
		size_t RedCrystal = data.first.find("@R1");
		size_t RedCrystal2 = data.first.find("@R2");
		size_t RedCrystal3 = data.first.find("@R3");
		size_t RedCrystal4 = data.first.find("@R4");
		size_t RedCrystal5 = data.first.find("@R5");
		size_t RedCrystal6 = data.first.find("@R6");
		size_t RedCrystal7 = data.first.find("@R7");
		size_t RedCrystal8 = data.first.find("@R8");

		if (tag != std::string::npos) {
			Mesh* mesh = Mesh::Get("...");
		}
		if (playerTag != std::string::npos) {

			SpawnPoint = render_data.models[0].getTranslation();
			player->playerMatrix.setTranslation(SpawnPoint);
			continue;
		}
		if (RedCrystal != std::string::npos || RedCrystal2 != std::string::npos ||
			RedCrystal3 != std::string::npos || RedCrystal4 != std::string::npos ||
			RedCrystal5 != std::string::npos || RedCrystal6 != std::string::npos ||
			RedCrystal7 != std::string::npos || RedCrystal8 != std::string::npos) {

			SpawnPoint = render_data.models[0].getTranslation();
			for (int i = 0; i < RED_CRISTALS_TOT; i++) {
				if (!Redcrystals[i]->active) {
					Redcrystals[i]->model.setTranslation(SpawnPoint);
					Redcrystals[i]->active = true;
					i = RED_CRISTALS_TOT;
				}
			}
			
			continue;
		}
		if (YellowCrystal != std::string::npos || YellowRed != std::string::npos) {
			SpawnPoint = render_data.models[0].getTranslation();
			for (int i = 0; i < YELLOW_CRISTALS_TOT; i++) {
				if (!Yellowcrystals[i]->active) {
					Yellowcrystals[i]->model.setTranslation(SpawnPoint);
					if (YellowRed != std::string::npos) {
						Yellowcrystals[i]->active = false;
						Yellowcrystals[i]->RedYellowCrystal = true;
					}
					else {
						Yellowcrystals[i]->active = true;
						Yellowcrystals[i]->finalCrystal = true;
					}
					i = YELLOW_CRISTALS_TOT;
				}
			}
			continue;
		}

		else {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());

			new_entity = new EntityCollider(mesh, mat, mesh_name);
		}

		if (!new_entity) {
			continue;
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			new_entity->isInstanced = true;
			new_entity->models = render_data.models; // Add all instances
		}
		// Create normal entity
		else {
			new_entity->model = render_data.models[0];
		}

		// Add entity to scene root
		
		root->addChild(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

void World::deleteRedCrystal(EntityCrystal* crystal)
{
	crystal->active = false;
	crystalsCollected += 1;
	//printf("%d\n", crystalsCollected);
	if (crystalsCollected == 1) {
		Audio::Play("data/sounds/red_coin_1.wav", 0.5);
	}
	else if (crystalsCollected == 2) {
		Audio::Play("data/sounds/red_coin_2.wav", 0.5);
	}
	else if (crystalsCollected == 3) {
		Audio::Play("data/sounds/red_coin_3.wav", 0.5);
	}
	else if (crystalsCollected == 4) {
		Audio::Play("data/sounds/red_coin_4.wav", 0.5);
	}
	else if (crystalsCollected == 5) {
		Audio::Play("data/sounds/red_coin_5.wav", 0.5);
	}
	else if (crystalsCollected == 6) {
		Audio::Play("data/sounds/red_coin_6.wav", 0.5);
	}
	else if (crystalsCollected == 7) {
		Audio::Play("data/sounds/red_coin_7.wav", 0.5);
	}
	else if (crystalsCollected == 8) {
		Audio::Play("data/sounds/red_coin_8.wav", 0.5);
		CrystalAnimation = true;
		Audio::Play("data/sounds/crystal_appears.wav", 0.5);
	}
}
void World::deleteYellowCrystal(EntityCrystal* crystal) 
{
	counter++;
	crystal->active = false;
	YellowCrystalCollectedAnimation = true;
	player->animator.playAnimation("data/animations/twerk.skanim");
	Audio::Play("data/sounds/got_crystal.wav", 0.5);

}

void World::animation_in_game(float delta_time) {
	for (int i = 0; i < YELLOW_CRISTALS_TOT; i++) {
		if (!Yellowcrystals[i]->active && Yellowcrystals[i]->RedYellowCrystal) {
			DirectionCrystal = Yellowcrystals[i]->model.getTranslation();
			DirectionCrystalCamera = Vector3(DirectionCrystal.x + 5, DirectionCrystal.y, DirectionCrystal.z + 5);
			Yellowcrystals[i]->active = true;
		}
	}
	if ((DirectionCrystalCamera.x > DirectionCrystal.x + 2) && (DirectionCrystalCamera.z > DirectionCrystal.z + 2)) {
		DirectionCrystalCamera.x -= 2 * delta_time;
		DirectionCrystalCamera.z -= 2 * delta_time;
	}
	else {
		if (CrystalAnimationTime > 2.0) {
			CrystalAnimation = false;
			CrystalAnimationTime = 0.0;
		}
		else {
			CrystalAnimationTime += 1 * delta_time;
		}
		
	}
	camera->lookAt(DirectionCrystalCamera, DirectionCrystal, Vector3(0, 1, 0));
}

void World::animation_get_crystal(float delta_time) 
{
	if (CrystalAnimationTime > 3.0) {
		showCrystalobtainedIcon = true;
		if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
			CrystalAnimationTime = 0.0;
			YellowCrystalCollectedAnimation = false;
			showCrystalobtainedIcon = false;
			for (int i = 0; i < YELLOW_CRISTALS_TOT; i++) {
				if (!Yellowcrystals[i]->active && Yellowcrystals[i]->finalCrystal) {
					Game::instance->course = NEXUS;
					Game::instance->goToStage(LOADING);
				}
			}
		}
	}
	else {
		CrystalAnimationTime += 1 * delta_time;
	}

}