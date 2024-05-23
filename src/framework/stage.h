#pragma once

#include "input.h"
#include "graphics/shader.h"
#include "world.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"

enum eStages {
	INTRO,
	PLAY,
	WIN,
	LOSE,
	STAGES_SIZE
};

class EntityUI;

class Stage {
public:

	Mesh* mesh_cube = NULL;
	Texture* texture_cube = NULL;
	Shader* shader_cube = NULL;
	Matrix44 m_cube;

	Camera* camera2D;

	EntityUI* background;
	EntityUI* playButton;

	virtual void onEnter() {};
	virtual void onExit() {};

	virtual void render() {}; // Empty body
	virtual void update(double seconds_elapsed) {}; // Empty body
};

class IntroStage : public Stage {

public:
	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class PlayStage : public Stage {

public:
	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class WinStage : public Stage {

public:
	virtual void onEnter();
	virtual void onExit();

	void render(Shader* shader);
	void update(double seconds_elapsed);
};

class LoseStage : public Stage {

public:
	virtual void onEnter();
	virtual void onExit();

	void render(Shader* shader);
	void update(double seconds_elapsed);
};
