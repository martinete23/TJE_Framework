#include "entitymesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include "framework/camera.h"
#include "framework/world.h"
#include "framework/input.h"
#include "game/game.h"

class EntityCollider;

EntityMesh::EntityMesh(Mesh* m, Material mat, std::string name)
{
	mesh = m;
	material = mat;
	this->name = name;
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

	idle = Animation::Get("data/animations/idle.skanim");
	run = Animation::Get("data/animations/run.skanim");
	jump = Animation::Get("data/animations/jump.skanim");
}

void EntityPlayer::render(Camera* camera)
{
	EntityMesh::render(camera);

	float sphere_radius = World::instance->sphere_radius;
	float sphere_ground_radius = World::instance->sphere_ground_radius;
	float player_height = World::instance->player_height;

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");
	Matrix44 m = playerMatrix;

	shader->enable();

	//draw the spheres
	//first
	{
		m.translate(0.0f, sphere_ground_radius, 0.0f);
		m.scale(sphere_ground_radius, sphere_ground_radius, sphere_ground_radius);

		shader->setUniform("u_color", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_model", m);

		//mesh->render(GL_LINES);
	}
	//second
	{
		m = playerMatrix;
		m.translate(0.0f, player_height, 0.0f);

		shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		m.scale(sphere_radius, sphere_radius, sphere_radius);
		shader->setUniform("u_model", m);

		//mesh->render(GL_LINES);
	}

	shader->disable();

	playerMaterial.shader->enable();

	playerMaterial.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	playerMaterial.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	playerMaterial.shader->setUniform("u_texture", playerMaterial.diffuse, 0);
	playerMaterial.shader->setUniform("u_model", playerMatrix);
	playerMaterial.shader->setUniform("u_time", time);

	if (state == IDLE)
	{
		idle->assignTime(Game::instance->time);
		playerMesh->renderAnimated(GL_TRIANGLES, &idle->skeleton);
	}
	else if (state == RUN)
	{
		run->assignTime(Game::instance->time);
		playerMesh->renderAnimated(GL_TRIANGLES, &run->skeleton);
	}
	else if (state == JUMP)
	{
		jump->assignTime(Game::instance->time);
		playerMesh->renderAnimated(GL_TRIANGLES, &jump->skeleton);
	}

	//playerMesh->render(GL_TRIANGLES);

	playerMaterial.shader->disable();
}

void EntityPlayer::update(float elapsed_time)
{
	/*if (isAnimated) {
		animator.update(elapsed_time);
	}*/

	float camera_yaw = World::instance->camera_yaw;

	Vector3 player_pos = playerMatrix.getTranslation();

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0,1,0));

	Vector3 move_dir;
	Vector3 character_front = mYaw.frontVector();
	Vector3 character_right = mYaw.rightVector();
	if (Input::isKeyPressed(SDL_SCANCODE_W))
	{
		move_dir += character_front;
		state = RUN;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S))
	{
		move_dir -= character_front;
		state = RUN;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_A))
	{
		move_dir += character_right;
		state = RUN;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D))
	{
		move_dir -= character_right;
		state = RUN;
	}

	if (!dashUse) {
		velocity += dashDirection * 3.0f;
	}

	if (isWallJumping && wallJumpTimer < 0.5f) {
		velocity -= moveDirection * 2.0f;
		wallJumpTimer += 1.0f * elapsed_time;
	}

	move_dir.normalize();
	move_dir *= 2.0f;
	velocity += move_dir;

	if (move_dir.x == 0.0f && move_dir.y == 0.0f && move_dir.z == 0.0f) {
		state = IDLE;
	}

	std::vector<sCollisionData> collisions;
	std::vector<sCollisionData> ground_collisions;

	World::instance->wallDetected = false;

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
			dashUse = true;

			if (boolJump == true) {
				World::instance->sphere_radius /= 2;
				boolJump = false;
				state = IDLE;
			}

			if (jumpTimer < 1.0f) {
				jumpTimer += 1.0f * elapsed_time;
			}
		}

		if (collision.col_point.y > (player_pos.y + velocity.y * elapsed_time)) {
			player_pos.y = collision.col_point.y;
		}
	}

	if (!is_grounded) {
		state = JUMP;
		velocity.y -= 9.8f * elapsed_time;
		jumpTimer = 0.0f;

		if (boolJump == false) {
			World::instance->sphere_radius *= 2;
			boolJump = true;
		}

		if (Input::wasKeyPressed(SDL_SCANCODE_LSHIFT) && dashUse == true) {
			dashUse = false;
			dashDirection = character_front;
		}
		if (Input::wasKeyPressed(SDL_SCANCODE_SPACE) && World::instance->wallDetected == true) {
			printf("wallJUMP\n");
			velocity.y = 6.0f;
			isWallJumping = true;
			moveDirection = move_dir;
			wallJumpTimer = 0.0f;
		}
	}
	else if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
		state = JUMP;
		if (jumpTimer < 0.2f) {
			velocity.y = 8.0f;
		}
		else {
			velocity.y = 5.0f;
		}
	}

	//printf("%f\n", jumpTimer);
	if (World::instance->wallDetected == true) {
		//printf("Wall Detected\n");
	}

	player_pos += velocity * elapsed_time;

	velocity.x *= 0.5f;
	velocity.z *= 0.5f;

	if (playerMatrix.getTranslation().y < 0) {
		playerMatrix.setTranslation(Vector3(0, 10, 0));
	}
	else {
		playerMatrix.setTranslation(player_pos);
	}

	playerMatrix.rotate(camera_yaw, Vector3(0, 1, 0));

	EntityMesh::update(elapsed_time);
}

void EntityPlayer::dash(float elapsed_time)
{
	float camera_yaw = World::instance->camera_yaw;
	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));
	Vector3 dash_direction = mYaw.frontVector(); 

}

void EntityCollider::getCollisionWithModel(const Matrix44& m, const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions)
{
	Vector3 collision_point;
	Vector3 collision_normal;
	Vector3 center = target_position;

	float sphere_radius = World::instance->sphere_radius;
	float sphere_ground_radius = World::instance->sphere_ground_radius;
	float player_height = World::instance->player_height;

	//floor collisions 
	Vector3 floor_sphere_center = center + Vector3(0.0f, sphere_ground_radius, 0.0f);

	if (mesh->testSphereCollision(m, floor_sphere_center, sphere_radius, collision_point, collision_normal)) {
		collisions.push_back({ collision_point, collision_normal.normalize(), floor_sphere_center.distance(collision_point) });
	}
	Vector3 character_center = center + Vector3(0.0f, player_height, 0.0f);
	if (mesh->testSphereCollision(m, character_center, sphere_radius, collision_point, collision_normal)) {
		World::instance->wallDetected = true;
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
