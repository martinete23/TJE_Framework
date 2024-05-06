#include "entitymesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "framework/camera.h"

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
			isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs", 
			material.diffuse ? "data/shaders/texture.fs" : "data/shaders/flat.fs");
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

