#include "entitymesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "framework/camera.h"
#include "framework/world.h"
#include "framework/input.h"


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
}

void EntityPlayer::render(Camera* camera)
{
	EntityMesh::render(camera);

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");
	Matrix44 m = model;

	shader->enable();

	m.translate(playerMatrix.getTranslation().x, playerMatrix.getTranslation().y + 0.5f, playerMatrix.getTranslation().z);
	m.scale(0.07, 0.07, 0.07);

	shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_model", m);

	mesh->render(GL_LINES);

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
