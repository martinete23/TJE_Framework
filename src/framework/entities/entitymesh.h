#include "entity.h"
#include <map>
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"



class EntityMesh : public Entity {

public:
	EntityMesh() {};
	EntityMesh(Mesh* m, Material mat, std::string name);

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

	bool dashUse = true;
	Vector3 dashDirection;
	float angle = 0;
	float jumpTimer = 0.0f;

	bool isWallJumping = false;
	Vector3 moveDirection;
	float wallJumpTimer = 0.0f;


	EntityPlayer() {};
	EntityPlayer(Mesh* m, Material mat);

	void render(Camera* camera);
	void update(float elapsed_time);
	void jump();
	void dash(float elapsed_time);
};

class EntityCollider : public EntityMesh
{
public:

	void getCollisionWithModel(const Matrix44& m, const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions);

	int layer = eCollisionFilter::SCENARIO;

	bool isStatic = true;

	EntityCollider() {};
	EntityCollider(Mesh* mesh, const Material& material, std::string name):
		EntityMesh(mesh, material, name) {};
	~EntityCollider() {};

	void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions, eCollisionFilter filter);

	int getLayer() { return layer; };
	void setLayer(int new_layer) { layer = new_layer; };
};