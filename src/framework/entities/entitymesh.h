#include "entity.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

class EntityMesh : public Entity {

public:
	EntityMesh() {};
	EntityMesh(Mesh* m, Material mat);

	// Attributes of the derived class  
	Mesh* mesh = nullptr;

	Material material;

	bool isInstanced = false;
	std::vector<Matrix44> models;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);

	void addInstance(const Matrix44& model);
};

class EntityPlayer : public EntityMesh {

public:

	Vector3 velocity = Vector3(0.0f);

	Matrix44 playerMatrix;
	Mesh* playerMesh;
	Material playerMaterial;
	bool hasJumped = false;

	EntityPlayer() {};
	EntityPlayer(Mesh* m, Material mat);

	void render(Camera* camera);
	void update(float elapsed_time);
	void jump();
};