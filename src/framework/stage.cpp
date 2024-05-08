#include "stage.h"
#include "game/game.h"
#include "framework/input.h"

void IntroStage::onEnter()
{
	texture_cube = Texture::Get("data/textures/StandardCubeMap.tga");

	mesh_cube = Mesh::Get("data/meshes/cubemap.obj");

	shader_cube = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
}

void IntroStage::onExit()
{
}

void IntroStage::render()
{
	World::instance->render();

	if (shader_cube)
	{
		// Enable shader
		shader_cube->enable();

		// Upload uniforms
		shader_cube->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader_cube->setUniform("u_viewprojection", World::instance->camera->viewprojection_matrix);
		shader_cube->setUniform("u_texture", texture_cube, 0);
		shader_cube->setUniform("u_model", m_cube);
		shader_cube->setUniform("u_time", time);


		// Do the draw call
		mesh_cube->render(GL_TRIANGLES);

		// Disable shader
		shader_cube->disable();
	}

}

void IntroStage::update(double seconds_elapsed)
{
	World::instance->update(seconds_elapsed);
	
}

void PlayStage::onEnter()
{
}

void PlayStage::onExit()
{
}

void PlayStage::render(Shader* shader)
{
}

void PlayStage::update(double seconds_elapsed)
{
}

void WinStage::onEnter()
{
}

void WinStage::onExit()
{
}

void WinStage::render(Shader* shader)
{
}

void WinStage::update(double seconds_elapsed)
{
}

void LoseStage::onEnter()
{
}

void LoseStage::onExit()
{
}

void LoseStage::render(Shader* shader)
{
}

void LoseStage::update(double seconds_elapsed)
{
}
