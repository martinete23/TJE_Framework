#pragma once
#include <map>
#include "graphics/material.h"
#include "framework/camera.h"

#define RED_CRISTALS_TOT 8

class Entity;
class EntityPlayer;
class EntityCrystal;

class World
{
public:
	static World* instance;
	World();

	Entity* root;
	Texture* texture;
	Camera* camera;

	EntityPlayer* player;

	EntityCrystal* crystals[RED_CRISTALS_TOT];
	int crystalsCollected;
	
	Vector3 SpawnPoint;

	float sphere_radius = 0.14f;
	float sphere_ground_radius = 0.07f;
	float player_height = 0.5f;

	float camera_yaw = 0.f;
	float camera_pitch = 0.f;
	float camera_speed = 2.0f;

	bool wallDetected = false;

	std::map<std::string, sRenderData> meshes_to_load;

	void render();
	void update(float delta_time);

	bool parseScene(const char* filename, Entity* root);

	void deleteCrystal(EntityCrystal* crystal);

	//sCollisionData raycast(const Vector3& origin, const Vector3& direction, int layer, float max_ray_dist, Entity* root);
};

