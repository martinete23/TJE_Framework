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
	PAUSE,
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
	EntityUI* exitCourseButton;

	EntityUI* icon0;
	EntityUI* icon1;
	EntityUI* icon2;
	EntityUI* icon3;
	EntityUI* icon4;
	EntityUI* crystal_obtained_icon;
	EntityUI* icon_RedCrystals;

	EntityUI* level2;
	EntityUI* level3;

	EntityUI* loading;
	float loading_time;

	HCHANNEL channel;

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

	Material icon_redCrystal_manterial;

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

class PauseStage : public Stage {

public:
	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};
