#pragma once
#include <map>
#include "graphics/material.h"

class Entity;
class Camera;

class World
{
public:
	static World* instance;
	World();

	Entity* root = nullptr;
	// EntityPlayer* player = nullptr;

	//Camera* camera = nullptr;

	std::map<std::string, sRenderData> meshes_to_load;

	void render();
	void update(float delta_time);

	bool parseScene(const char* filename, Entity* root);
};

