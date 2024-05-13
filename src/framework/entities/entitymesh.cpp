#include "entitymesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include "framework/camera.h"
#include "framework/world.h"
#include "framework/input.h"

class EntityCollider;

EntityMesh::EntityMesh(Mesh* m, Material mat)
{
	mesh = m;
	material = mat;
}

void EntityMesh::render(Camera* camera)
{
	if (!mesh) return;

	Vector3 center_world = model * mesh->box.center;
	float aabb_radius = mesh->box.halfsize.length();

	if (camera->testSphereInFrustum(center_world, aabb_radius) == false)
	{
		return;
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	if (!material.shader)
	{
		material.shader = Shader::Get(
			isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs", "data/shaders/texture.fs");
	}



	// Get the last camera that was activated 
	// Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	material.shader->enable();
	material.shader->setUniform("u_model", getGlobalMatrix());
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setUniform("u_color", Vector4(1,1,1,1));

	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0);
	}

	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material.shader->disable();

	for (int i = 0; i < children.size(); ++i)
	{
		children[i]->render(camera);
	}
}
void EntityMesh::update(float elapsed_time)
{
	for (int i = 0; i < children.size(); ++i)
	{
		children[i]->update(elapsed_time);
	}
}

void EntityMesh::addInstance(const Matrix44& model)
{
	models.push_back(model);
}

EntityPlayer::EntityPlayer(Mesh* m, Material mat)
{
	playerMesh = m;
	playerMaterial = mat;

	playerMatrix.setTranslation(Vector3(0,10,0));
}

void EntityPlayer::render(Camera* camera)
{
	EntityMesh::render(camera);

	float sphere_radius = World::instance->sphere_radius;
	float sphere_ground_radius = World::instance->spehre_ground_radius;
	float player_height = World::instance->player_height;

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");
	Matrix44 m = playerMatrix;

	shader->enable();

	//draw the spheres
	//first
	{
		m.translate(0.0f, sphere_ground_radius, 0.0f);
		m.scale(sphere_ground_radius, sphere_ground_radius, sphere_radius);

		shader->setUniform("u_color", Vector4(1, 0, 0, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_model", m);

		mesh->render(GL_LINES);
	}
	//second
	{
		m = playerMatrix;
		m.translate(0.0f, player_height, 0.0f);

		shader->setUniform("u_color", Vector4(0, 1, 0, 1));
		m.scale(sphere_radius, sphere_radius, sphere_radius);
		shader->setUniform("u_model", m);

		mesh->render(GL_LINES);
	}

	shader->disable();

	playerMaterial.shader->enable();

	playerMaterial.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	playerMaterial.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	playerMaterial.shader->setUniform("u_texture", playerMaterial.diffuse, 0);
	playerMaterial.shader->setUniform("u_model", playerMatrix);
	playerMaterial.shader->setUniform("u_time", time);
	playerMesh->render(GL_TRIANGLES);

	playerMaterial.shader->disable();
}

void EntityPlayer::update(float elapsed_time)
{
	float camera_yaw = World::instance->camera_yaw;

	Vector3 player_pos = playerMatrix.getTranslation();

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0,1,0));

	Vector3 move_dir;
	Vector3 character_front = mYaw.frontVector();
	Vector3 character_right = mYaw.rightVector();

	if (Input::isKeyPressed(SDL_SCANCODE_UP)) move_dir += character_front;
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) move_dir -= character_front;
	if (Input::isKeyPressed(SDL_SCANCODE_LEFT)) move_dir += character_right;
	if (Input::isKeyPressed(SDL_SCANCODE_RIGHT)) move_dir -= character_right;

	move_dir.normalize();
	move_dir *= 3.0f;
	velocity += move_dir;

	std::vector<sCollisionData> collisions;
	std::vector<sCollisionData> ground_collisions;

	for (auto e : World::instance->root->children) {
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (ec != nullptr) {
			ec->getCollisions(player_pos + velocity * elapsed_time, collisions, ground_collisions, SCENARIO);
		}
	}

	for (const sCollisionData& collision : collisions) {
		Vector3 newDir = velocity.dot(collision.col_normal) * collision.col_normal;
		velocity.x -= newDir.x;
		velocity.y -= newDir.y;
		velocity.z -= newDir.z;
	}

	bool is_grounded = false;

	for (const sCollisionData& collision : ground_collisions) {
		float up_factor = fabsf(collision.col_normal.dot(Vector3::UP));
		if (up_factor > 0.8) {
			is_grounded = true;
			if (hasJumped && timerDetect > 10.0f) {
				printf("ground");
				hasJumped = false;
				timerJump = 0.0f;
			}
		}

		if (collision.col_point.y > (player_pos.y + velocity.y * elapsed_time)) {
			player_pos.y = collision.col_point.y;
		}
	}

	if (!is_grounded) {
		velocity.y -= 9.8f * elapsed_time;
	}
	else if (Input::isKeyPressed(SDL_SCANCODE_Z)) {
		if (timerJump < 200.f) {
			velocity.y = 8.0f;
		}
		else {
			velocity.y = 5.0f;
		}
		hasJumped = true;
		timerDetect = 0.0f;
	}
	if (timerJump < 300.0f) {
		timerJump += 1.0f;
		printf("%f\n", timerJump);
	}
	if (timerDetect < 50.0f) {
		timerDetect += 1.0f;
	}


	player_pos += velocity * elapsed_time;

	velocity.x *= 0.5f;
	velocity.z *= 0.5f;

	playerMatrix.setTranslation(player_pos);
	playerMatrix.rotate(camera_yaw, Vector3(0, 1, 0));

	EntityMesh::update(elapsed_time);
}

void EntityPlayer::jump()
{
}

void EntityCollider::getCollisionWithModel(const Matrix44& m, const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions)
{
	Vector3 collision_point;
	Vector3 collision_normal;
	Vector3 center = target_position;

	
	float sphere_radius = World::instance->sphere_radius;
	float sphere_ground_radius = World::instance->spehre_ground_radius;
	float player_height = World::instance->player_height;

	//floor collisions 
	Vector3 floor_sphere_center = center + Vector3(0.0f, sphere_ground_radius, 0.0f);

	if (mesh->testSphereCollision(m, floor_sphere_center, sphere_radius, collision_point, collision_normal)) {
		collisions.push_back({ collision_point, collision_normal.normalize(), floor_sphere_center.distance(collision_point) });
	}
	Vector3 character_center = center + Vector3(0.0f, player_height, 0.0f);
	if (mesh->testSphereCollision(m, character_center, sphere_radius, collision_point, collision_normal)) {
		collisions.push_back({ collision_point, collision_normal.normalize(), character_center.distance(collision_point) });
	}
	if (mesh->testRayCollision(m, character_center, Vector3(0, -1, 0), collision_point, collision_normal, player_height + 0.01f)) {
		ground_collisions.push_back({collision_point, collision_normal.normalize(), character_center.distance(collision_point)});
	}

}

void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions, eCollisionFilter filter)
{
	if (!(layer & filter))
		return;

	if (!isInstanced) {
		getCollisionWithModel(model, target_position, collisions, ground_collisions);
	}
	else {
		for (int i = 0; i < models.size(); i++) {
			getCollisionWithModel(models[i], target_position, collisions, ground_collisions);
		}
	}
}
