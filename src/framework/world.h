#pragma once
#include <map>
#include "graphics/material.h"
#include "framework/camera.h"

class Entity;
class EntityPlayer;

class World
{
public:
	static World* instance;
	World();

	Entity* root;
	Texture* extrue;
	Camera* camera;
	EntityPlayer* player;
	
	float sphere_radius = 0.07f;
	float spehre_ground_radius = 0.07f;
	float player_height = 0.5f;

	float camera_yaw = 0.f;
	float camera_pitch = 0.f;
	float camera_speed = 2.0f;

	std::map<std::string, sRenderData> meshes_to_load;

	void render();
	void update(float delta_time);

	bool parseScene(const char* filename, Entity* root);
};

