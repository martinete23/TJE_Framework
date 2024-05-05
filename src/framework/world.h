#pragma once
#include <map>
#include "graphics/material.h"
#include "entities/entitymesh.h"

class Entity;
class Camera;

class World
{
public:
	static World* instance;
	World();

	Entity* root = nullptr;
	Texture* extrue;
	//Camera* camera = nullptr;
	EntityPlayer* player;

	std::map<std::string, sRenderData> meshes_to_load;

	void render();
	void update(float delta_time);

	bool parseScene(const char* filename, Entity* root);
};

