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

	crystal = new EntityCrystal(Mesh::Get("data/meshes/red_cristal.obj"), crystal_material);

	if (Game::instance->course == TUTORIAL) {
		parseScene("data/Tutorial.scene", root);
	}
	if (Game::instance->course == LEVEL1) {
		parseScene("data/myscene2.scene", root);
	}
}

void World::render()
{
	camera->enable();

	root->render(camera);

	player->render(camera);

	crystal->render(camera);
}

void World::update(float delta_time)
{
	root->update(delta_time);
	player->update(delta_time);
	crystal->update(delta_time);

	if (Input::isKeyPressed(SDL_SCANCODE_K))
	{
		crystal->~EntityCrystal();
	}

	camera_yaw -= Input::mouse_delta.x * delta_time * 0.08f;
	camera_pitch -= Input::mouse_delta.y * delta_time * 0.08f;

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

		if (tag != std::string::npos) {
			Mesh* mesh = Mesh::Get("...");
		}
		if (playerTag != std::string::npos) {

			SpawnPoint = render_data.models[0].getTranslation();
			player->playerMatrix.setTranslation(SpawnPoint);
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

//sCollisionData World::raycast(const Vector3& origin, const Vector3& direction, int layer, float max_ray_dist, Entity* root)
//{
//	sCollisionData data;
//
//	for (auto e : root->children)
//	{
//		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
//		if (ec == nullptr || !(ec->getLayer() & layer)) {
//			continue;
//		}
//
//		Vector3 col_point;
//		Vector3 col_normal;
//
//		if (!ec->mesh->testRayCollision(ec->model, origin, direction, col_point, col_normal, max_ray_dist)) {
//			continue;
//		}
//
//		data.collided = true;
//
//		float max_distance = (col_point - origin).length();
//		if (max_distance < data.distance) {
//			data.col_point = col_point;
//			data.col_normal = col_normal;
//			data.distance = max_distance;
//		}
//	}
//
//	return data;
//}