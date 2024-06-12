#pragma once
#include <map>
#include "graphics/material.h"
#include "framework/camera.h"

#define RED_CRISTALS_TOT 8
#define YELLOW_CRISTALS_TOT 2

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

	EntityCrystal* Redcrystals[RED_CRISTALS_TOT];

	EntityCrystal* Yellowcrystals[YELLOW_CRISTALS_TOT];
	bool CrystalAnimation = false;
	Vector3 DirectionCrystal;
	Vector3 DirectionCrystalCamera;
	float CrystalAnimationTime = 0.0;

	int crystalsCollected;

	bool YellowCrystalCollected = false;
	bool YellowCrystalCollectedAnimation = false;
	bool showCrystalobtainedIcon = false;
	
	Vector3 SpawnPoint;

	float sphere_radius = 0.14f;
	float sphere_ground_radius = 0.09f;
	float player_height = 0.5f;

	float camera_yaw = 0.f;
	float camera_pitch = 0.f;
	float camera_speed = 2.0f;

	bool wallDetected = false;

	std::map<std::string, sRenderData> meshes_to_load;

	void render();
	void update(float delta_time);

	bool parseScene(const char* filename, Entity* root);

	void deleteRedCrystal(EntityCrystal* crystal);
	void deleteYellowCrystal(EntityCrystal* crystal);
	
	void animation_in_game(float delta_time);
	void animation_get_crystal(float delta_time);

	//sCollisionData raycast(const Vector3& origin, const Vector3& direction, int layer, float max_ray_dist, Entity* root);
};

