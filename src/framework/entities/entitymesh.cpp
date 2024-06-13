
#include "entitymesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include "framework/camera.h"
#include "framework/world.h"
#include "framework/input.h"
#include "game/game.h"

class EntityCollider;
bool RedCrystal_collided = false;
bool YellowCrystal_collided = false;

EntityMesh::EntityMesh(Mesh* m, Material mat, std::string name)
{
	mesh = m;
	material = mat;
	this->name = name;
}

EntityMesh::~EntityMesh()
{
	delete mesh;
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
	material.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	material.shader->setUniform("u_scale", 20.0f);

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

	animator.playAnimation("data/animations/idle.skanim");
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

	{
		m.translate(0.0f, sphere_ground_radius, 0.0f);
		m.scale(sphere_ground_radius, sphere_ground_radius, sphere_ground_radius);

		shader->setUniform("u_color", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_model", m);

		//mesh->render(GL_TRIANGLES);
	}
	{
		m = playerMatrix;
		m.translate(0.0f, player_height, 0.0f);

		shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		m.scale(sphere_radius, sphere_radius, sphere_radius);
		shader->setUniform("u_model", m);

		//mesh->render(GL_TRIANGLES);
	}

	shader->disable();

	playerMaterial.shader->enable();

	playerMaterial.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	playerMaterial.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	playerMaterial.shader->setUniform("u_texture", playerMaterial.diffuse, 0);
	playerMaterial.shader->setUniform("u_model", playerMatrix);
	playerMaterial.shader->setUniform("u_scale", 1.0f);
	playerMaterial.shader->setUniform("u_time", time);
	//playerMaterial.shader->setUniform("u_ka", Vector3);
	//playerMaterial.shader->setUniform("light_position", Vector3);
	//playerMaterial.shader->setUniform("u_light_color", Vector3);

	playerMesh->renderAnimated(GL_TRIANGLES, &animator.getCurrentSkeleton());

	playerMaterial.shader->disable();
}

void EntityPlayer::update(float elapsed_time)
{
	front = false;
	back = false;
	right = false;
	left = false;

	if (isAnimated) {
		animator.update(elapsed_time);
	}

	float camera_yaw = World::instance->camera_yaw;

	Vector3 player_pos = playerMatrix.getTranslation();

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

	Vector3 move_dir;
	Vector3 character_front = mYaw.frontVector();
	Vector3 character_right = mYaw.rightVector();

	if (Input::isKeyPressed(SDL_SCANCODE_W) || (Input::gamepads->axis[LEFT_ANALOG_Y] <= -1))
	{
		move_dir += character_front;
		front = true;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::gamepads->axis[LEFT_ANALOG_Y] > 0)
	{
		move_dir -= character_front;
		back = true;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::gamepads->axis[LEFT_ANALOG_X] < 0)
	{
		move_dir += character_right;
		left = true;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::gamepads->axis[LEFT_ANALOG_X] > 0)
	{
		move_dir -= character_right;
		right = true;
	}

	if (hasDashed) {
		velocity += dashDirection * 3.0f;
	}

	if (isWallJumping && wallJumpTimer < 0.8f) {
		velocity -= moveDirection * 1.5f;
		wallJumpTimer += 1.0f * elapsed_time;
	}

	move_dir.normalize();
	move_dir *= 2.0f;
	velocity += move_dir;

	std::vector<sCollisionData> collisions;
	std::vector<sCollisionData> ground_collisions;

	World::instance->wallDetected = false;

	for (auto e : World::instance->root->children) {
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (ec != nullptr) {
			ec->getCollisions(player_pos + velocity * elapsed_time, collisions, ground_collisions, SCENARIO);
		}
	}

	for (int i = 0; i < RED_CRISTALS_TOT; i++) {
		if (World::instance->Redcrystals[i]->active) {
			RedCrystal_collided = false;
			World::instance->Redcrystals[i]->getCollisions(player_pos + velocity * elapsed_time, collisions, ground_collisions, SCENARIO);
			if (RedCrystal_collided) {
				World::instance->deleteRedCrystal(World::instance->Redcrystals[i]);
			}
		}
	}
	for (int i = 0; i < YELLOW_CRISTALS_TOT; i++) {
		if (World::instance->Yellowcrystals[i]->active) {
			YellowCrystal_collided = false;
			World::instance->Yellowcrystals[i]->getCollisions(player_pos + velocity * elapsed_time, collisions, ground_collisions, SCENARIO);
			if (YellowCrystal_collided) {
				World::instance->deleteYellowCrystal(World::instance->Yellowcrystals[i]);
			}
		}
	}

	for (const sCollisionData& collision : collisions) {
		Vector3 newDir = velocity.dot(collision.col_normal) * collision.col_normal;
		velocity.x -= newDir.x;
		velocity.y -= newDir.y;
		velocity.z -= newDir.z;
	}

	is_grounded = false;

	for (const sCollisionData& collision : ground_collisions) {

		float up_factor = fabsf(collision.col_normal.dot(Vector3::UP));
		if (up_factor > 0.8) {
			is_grounded = true;
			hasDashed = false;
			canDash = true;

			if (boolJump == true) {
				//World::instance->sphere_radius /= 2.2;
				boolJump = false;
			}

			if (jumpTimer < 1.0f) {
				jumpTimer += 1.0f * elapsed_time;
			}
		}

		if (collision.col_point.y > (player_pos.y + velocity.y * elapsed_time)) {
			player_pos.y = collision.col_point.y;
		}
	}

	if (is_grounded)
	{
		if (state == IDLE && Input::wasKeyPressed(SDL_SCANCODE_T))
		{
			animator.playAnimation("data/animations/twerk.skanim");
		}
		if (move_dir.x == 0.0f && move_dir.y == 0.0f && move_dir.z == 0.0f) {
			if (state != IDLE)
			{
				animator.playAnimation("data/animations/idle.skanim");
				state = IDLE;
			}
		}
		if (state != RUN_RIGHT && right && (front || back))
		{
			animator.playAnimation("data/animations/run_right.skanim");
			state = RUN_RIGHT;
		}
		else if (state != RUN_LEFT && left && (front || back))
		{
			animator.playAnimation("data/animations/run_left.skanim");
			state = RUN_LEFT;
		}
		else if (state != RUN_FRONT && front && !right && !left)
		{
			animator.playAnimation("data/animations/run.skanim");
			state = RUN_FRONT;
		}
		else if (state != RUN_BACK && back && !right && !left)
		{
			animator.playAnimation("data/animations/run_back.skanim");
			state = RUN_BACK;
		}
		else if (state != RUN_RIGHT && right)
		{
			animator.playAnimation("data/animations/run_right.skanim");
			state = RUN_RIGHT;
		}
		else if (state != RUN_LEFT && left)
		{
			animator.playAnimation("data/animations/run_left.skanim");
			state = RUN_LEFT;
		}
	}

	if (!is_grounded) {

		velocity.y -= 9.8f * elapsed_time;
		jumpTimer = 0.0f;

		if (state != JUMP && state != FALL && state != DASH)
		{
			animator.playAnimation("data/animations/fall.skanim");
			state = FALL;
		}

		if (boolJump == false) {
			//World::instance->sphere_radius *= 2.2;
			boolJump = true;
		}

		if ((Input::wasKeyPressed(SDL_SCANCODE_LSHIFT) || Input::gamepads->wasButtonPressed(X_BUTTON)) && canDash == true){
			canDash = false;
			hasDashed = true;
			dashDirection = character_front;
			if (state != DASH)
			{
				animator.playAnimation("data/animations/dash.skanim");
				state = DASH;
				Audio::Play("data/sounds/yahoo.wav", 0.5);
			}
		}
		if ((Input::wasKeyPressed(SDL_SCANCODE_SPACE) || Input::gamepads->isButtonPressed(A_BUTTON)) && World::instance->wallDetected == true) {
			hasDashed = false;
			velocity.y = 6.0f;
			isWallJumping = true;
			moveDirection = move_dir;
			wallJumpTimer = 0.0f;
			Audio::Play("data/sounds/hoohoo.wav", 0.5);
		}
	}
	else if ((Input::isKeyPressed(SDL_SCANCODE_SPACE)) || Input::gamepads->isButtonPressed(A_BUTTON)) {
		jump();
	}

	player_pos += velocity * elapsed_time;

	velocity.x *= 0.5f;
	velocity.z *= 0.5f;

	if (playerMatrix.getTranslation().y < 0) {
		Game::instance->goToStage(LOSE);
		playerMatrix.setTranslation(World::instance->SpawnPoint);
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

void EntityPlayer::jump()
{
	if (state != JUMP)
	{
		animator.playAnimation("data/animations/fall.skanim");
		state = JUMP;
	}
	if (jumpTimer < 0.2f) {
		velocity.y = 8.0f;
		Audio::Play("data/sounds/waha.wav", 0.5);
	}
	else {
		velocity.y = 5.0f;
		Audio::Play("data/sounds/hoohoo.wav", 0.5);
	}
}

void EntityPlayer::animationUpdate(float elapsed_time)
{
	if (isAnimated) {
		animator.update(elapsed_time);
	}
	EntityMesh::update(elapsed_time);
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
		if (this->name == "red_crystal") {
			RedCrystal_collided = true;
		}
		else if (this->name == "yellow_crystal") {
			YellowCrystal_collided = true;
		}
		else {
			collisions.push_back({ collision_point, collision_normal.normalize(), floor_sphere_center.distance(collision_point) });
		}
	}
	Vector3 character_center = center + Vector3(0.0f, player_height, 0.0f);
	if (mesh->testSphereCollision(m, character_center, sphere_radius, collision_point, collision_normal)) {
		if (this->name == "red_crystal") {
			RedCrystal_collided = true;
		}
		else if (this->name == "yellow_crystal") {
			YellowCrystal_collided = true;
		}
		else if (this->name == "scene/Portal_Nexus/Portal_Nexus.obj") {
			Audio::Play("data/sounds/portal_spawn.wav", 0.5);
			World::instance->Portal_Animated = true;
		}
		else if (this->name == "scene/Level1Portal/Level1Portal.obj") {
			Game::instance->course = LEVEL1;
			Game::instance->goToStage(LOADING);
		}
		else if (this->name == "scene/Level2Portal/Level2Portal.obj") {
			if (Game::instance->CrystalCounter >= 1) {
				Game::instance->course = LEVEL2;
				Game::instance->goToStage(LOADING);
			} else {
				Game::instance->displayImage = true;
			}
		}
		else if (this->name == "scene/Level3Portal/Level3Portal.obj") {
			if (Game::instance->CrystalCounter >= 2) {
				Game::instance->course = LEVEL3;
				Game::instance->goToStage(LOADING);
			} else {
				Game::instance->displayImage2 = true;
			}
		}
		else {
			World::instance->wallDetected = true;
			//printf("wall Detected");
			collisions.push_back({ collision_point, collision_normal.normalize(), character_center.distance(collision_point) });
		}
	}
	if (mesh->testRayCollision(m, character_center, Vector3(0, -1, 0), collision_point, collision_normal, player_height + 0.01f)) {
		if (this->name == "red_crystal") {
			RedCrystal_collided = true;
		}
		else if (this->name == "yellow_crystal") {
			YellowCrystal_collided = true;
		}
		else {
			ground_collisions.push_back({ collision_point, collision_normal.normalize(), character_center.distance(collision_point) });
		}
	}
}

sCollisionData EntityCollider::raycast(const Vector3& origin, const Vector3& direction, int layer, float max_ray_dist)
{
	sCollisionData data;
	data.collided = false;
	data.distance = max_ray_dist;

	for (auto e : World::instance->root->children)
	{
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (ec == nullptr || !(ec->getLayer() & layer)) {
			continue;
		}

		Vector3 col_point;
		Vector3 col_normal;

		if (!ec->mesh->testRayCollision(ec->model, origin, direction, col_point, col_normal, max_ray_dist)) {
			continue;
		}

		data.collided = true;

		float max_distance = (col_point - origin).length();
		if (max_distance < data.distance) {
			data.col_point = col_point;
			data.col_normal = col_normal;
			data.distance = max_distance;
		}
	}

	return data;
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

EntityUI::EntityUI(Vector2 size, const Material& material)
{
	this->size = size;

	mesh = new Mesh();
	mesh->createQuad(Game::instance->window_width / 2, Game::instance->window_height / 2, size.x, size.y, true);

	this->material = material;

	if (!this->material.shader) {
		this->material.shader = Shader::Get("data/shaders/basic.vs", material.diffuse ? "data/shaders/texture.fs" : "data/shaders/flat.fs");
	}

}

EntityUI::EntityUI(Vector2 pos, Vector2 size, const Material& material)
{
	this->position = pos;
	this->size = size;

	mesh = new Mesh();
	mesh->createQuad(pos.x, pos.y, size.x, size.y, true);
	this->material = material;
	this->button_id = BACKGROUND;

	if (!this->material.shader) {
		this->material.shader = Shader::Get("data/shaders/basic.vs", material.diffuse ? "data/shaders/texture.fs" : "data/shaders/flat.fs");
	}

}

EntityUI::EntityUI(Vector2 pos, Vector2 size, const Material& material, eButtonID button_id, const std::string& name)
{
	this->position = pos;
	this->size = size;

	mesh = new Mesh();
	mesh->createQuad(pos.x, pos.y, size.x, size.y, true);
	this->material = material;

	this->name = name;
	this->button_id = button_id;

	if (!this->material.shader) {
		this->material.shader = Shader::Get("data/shaders/basic.vs", material.diffuse ? "data/shaders/texture.fs" : "data/shaders/flat.fs");
	}


}

void EntityUI::render(Camera* camera2D)
{

	if (material.shader)
	{
		// Enable shader
		material.shader->enable();

		// Upload uniforms

		material.shader->setUniform("u_model", getGlobalMatrix());
		material.shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
		material.shader->setUniform("u_color", material.color);
		material.shader->setUniform("u_scale", mask);


		if (material.diffuse) {
			material.shader->setTexture("u_texture", material.diffuse, 0);
		}

		// Do the draw call
		mesh->render(GL_TRIANGLES);

		// Disable shader
		material.shader->disable();
	}

	//glClear(GL_DEPTH_BUFFER_BIT);

	Entity::render(camera2D);

}

void EntityUI::update(float elapsed_time)
{
	Vector2 mouse_pos = Input::mouse_position;

	if (button_id == BUTTONPLAY || button_id == BUTTONTUTORIAL || button_id == BUTTONLEVEL1 || button_id == BUTTONLEVEL2 || button_id == BUTTONLEVEL3) {
		if ((mouse_pos.x >= position.x - size.x / 2) && (mouse_pos.x <= position.x + size.x / 2) &&
			(mouse_pos.y >= position.y - size.y / 2) && (mouse_pos.y <= position.y + size.y / 2)) {
			material.color = Vector4(1, 0, 0, 1);
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				if (button_id == BUTTONPLAY) {
					Game::instance->course = TUTORIAL;
					Game::instance->goToStage(LOADING);
				}
				else if (button_id == BUTTONTUTORIAL) {
					Game::instance->course = TUTORIAL;
					Game::instance->goToStage(LOADING);
				}
				else if (button_id == BUTTONLEVEL1) {
					Game::instance->course = LEVEL1;
					Game::instance->goToStage(LOADING);
				}
				else if (button_id == BUTTONLEVEL2) {
					Game::instance->course = LEVEL2;
					Game::instance->goToStage(LOADING);
				}
				else if (button_id == BUTTONLEVEL3) {
					Game::instance->course = LEVEL3;
					Game::instance->goToStage(LOADING);
				}
			}
		}
		else {
			material.color = Vector4(1, 1, 1, 1);
		}
	}
	if (button_id == PLAYAGAIN) {
		if ((mouse_pos.x >= position.x - size.x / 2) && (mouse_pos.x <= position.x + size.x / 2) &&
			(mouse_pos.y >= position.y - size.y / 2) && (mouse_pos.y <= position.y + size.y / 2)) {
			material.color = Vector4(1, 0, 0, 1);
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				if (Game::instance->course == TUTORIAL) {
					Game::instance->goToStage(PLAY);
				}
				else {
					Game::instance->course = NEXUS;
					Game::instance->goToStage(LOADING);
				}
			}
		}
		else {
			material.color = Vector4(1, 1, 1, 1);
		}
	}
	if (button_id == CONTINUE) {
		if ((mouse_pos.x >= position.x - size.x / 2) && (mouse_pos.x <= position.x + size.x / 2) &&
			(mouse_pos.y >= position.y - size.y / 2) && (mouse_pos.y <= position.y + size.y / 2)) {
			material.color = Vector4(1, 0, 0, 1);
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				Game::instance->goToStage(PLAY);
			}
		}
		else {
			material.color = Vector4(1, 1, 1, 1);
		}
	}
	if (button_id == BUTTONQUIT) {
		if ((mouse_pos.x >= position.x - size.x / 2) && (mouse_pos.x <= position.x + size.x / 2) &&
			(mouse_pos.y >= position.y - size.y / 2) && (mouse_pos.y <= position.y + size.y / 2)) {
			material.color = Vector4(1, 0, 0, 1);
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				Game::instance->must_exit = true;
			}
		}
		else {
			material.color = Vector4(1, 1, 1, 1);
		}
	}
	if (button_id == BUTTONEXITCOURSE) {
		if ((mouse_pos.x >= position.x - size.x / 2) && (mouse_pos.x <= position.x + size.x / 2) &&
			(mouse_pos.y >= position.y - size.y / 2) && (mouse_pos.y <= position.y + size.y / 2)) {
			material.color = Vector4(1, 0, 0, 1);
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				Game::instance->goToStage(LOADING);
				Game::instance->course = NEXUS;
			}
		}
		else {
			material.color = Vector4(1, 1, 1, 1);
		}
	}
	if (button_id == BUTTONCONTROLLER) {
		if ((mouse_pos.x >= position.x - size.x / 2) && (mouse_pos.x <= position.x + size.x / 2) &&
			(mouse_pos.y >= position.y - size.y / 2) && (mouse_pos.y <= position.y + size.y / 2)) {
			material.color = Vector4(1, 0, 0, 1);
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				Game::instance->goToStage(TUTORIALIMAGE);
			}
		}
		else {
			material.color = Vector4(1, 1, 1, 1);
		}
	}
	if (button_id == BUTTONBACK) {
		if ((mouse_pos.x >= position.x - size.x / 2) && (mouse_pos.x <= position.x + size.x / 2) &&
			(mouse_pos.y >= position.y - size.y / 2) && (mouse_pos.y <= position.y + size.y / 2)) {
			material.color = Vector4(1, 0, 0, 1);
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				Game::instance->goToStage(INTRO);
			}
		}
		else {
			material.color = Vector4(1, 1, 1, 1);
		}
	}
	Entity::update(elapsed_time);
}

EntityCrystal::EntityCrystal(Mesh* m, Material mat, std::string name) : EntityCollider(m, mat, name)
{
	active = false;
	finalCrystal = false;
	RedYellowCrystal = false;
}

EntityCrystal::~EntityCrystal()
{
	delete this->mesh;
}

void EntityCrystal::render(Camera* camera)
{
	this->material.shader->enable();

	this->material.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	this->material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	this->material.shader->setUniform("u_texture", this->material.diffuse, 0);
	this->material.shader->setUniform("u_model", this->model);
	this->material.shader->setUniform("u_scale", 1.0f);
	this->material.shader->setUniform("u_time", time);

	this->mesh->render(GL_TRIANGLES);

	this->material.shader->disable();
}

void EntityCrystal::update(float elapsed_time)
{
	this->model.rotate(elapsed_time, Vector3(0.0f, 1.0f, 0.0f));
}
