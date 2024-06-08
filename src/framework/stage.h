#pragma once

#include "input.h"
#include "graphics/shader.h"
#include "world.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "audio.h"

enum eStages {
	INTRO,
	PLAY,
	WIN,
	LOSE,
	LOADING,
	STAGES_SIZE
};

class EntityUI;
class EntityMesh;

class Stage {
public:

	EntityMesh* skybox;
	Mesh mesh_cube;
	Material texture_cube;
	Matrix44 m_cube;

	Camera* camera2D;

	EntityUI* background;
	EntityUI* playButton;
	EntityUI* quitButton;
	EntityUI* playAgainButton;

	EntityUI* icon;
	EntityUI* crystal_obtained_icon;

	EntityUI* loading;
	float loading_time;

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

	HCHANNEL channel;

	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class LoadingStage : public Stage {

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

	void render();
	void update(double seconds_elapsed);
};

class LoseStage : public Stage {

public:
	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};
