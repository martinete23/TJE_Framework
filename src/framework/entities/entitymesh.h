#include "entity.h"
#include <map>
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "framework/animation.h"

enum eButtonID {
	BUTTONPLAY = 0,
	BUTTONQUIT = 1,
};


enum eAnimationState {
	IDLE,
	RUN_FRONT,
	RUN_RIGHT,
	RUN_LEFT,
	RUN_BACK,
	JUMP,
	FALL,
	DASH
};

class EntityMesh : public Entity {

public:
	EntityMesh() {};
	EntityMesh(Mesh* m, Material mat, std::string name);

	// Attributes of the derived class  
	Mesh* mesh = nullptr;

	Animator animator;

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

	bool boolJump = false;

	bool dashUse = true;
	Vector3 dashDirection;
	float angle = 0;
	float jumpTimer = 0.0f;

	bool isWallJumping = false;
	Vector3 moveDirection;
	float wallJumpTimer = 0.0f;

	float jumpingTime;

	int state = IDLE;

	bool isAnimated;

	bool front;
	bool back;
	bool right;
	bool left;

	EntityPlayer() {};
	EntityPlayer(Mesh* m, Material mat);

	void render(Camera* camera);
	void update(float elapsed_time);
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

class EntityUI : public EntityMesh
{

public:
	EntityUI(Vector2 size, const Material& material);
	EntityUI(Vector2 pos, Vector2 size, const Material& material, eButtonID button_id, const std::string& name);

	~EntityUI() {};

	Vector2 position;
	Vector2 size;

	eButtonID button_id;

	bool visible = true;

	float mask = 1.0f;

	void render(Camera* camera2D);
	void update(float elapsed_time);
};