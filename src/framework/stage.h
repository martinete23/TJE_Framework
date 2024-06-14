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
	TUTORIALIMAGE,
	COURSESELECT,
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

	EntityUI* loading;
	float loading_time;

	std::vector<Texture*> animationFrames; // Animation frames
	int currentFrame = 0;
	float animationSpeed = 10.0f; // Frames per second
	float timeSinceLastFrame = 0;

	HCHANNEL channel;

	virtual void onEnter() {};
	virtual void onExit() {};

	virtual void render() {}; // Empty body
	virtual void update(double seconds_elapsed) {}; // Empty body
};

class IntroStage : public Stage {

public:

	EntityUI* playButton;
	EntityUI* quitButton;
	EntityUI* tutorialButton;

	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class PlayStage : public Stage {

public:

	EntityUI* icon0;
	EntityUI* icon1;
	EntityUI* icon2;
	EntityUI* icon3;
	EntityUI* icon4;
	EntityUI* icon5;
	EntityUI* icon6;
	EntityUI* icon7;
	EntityUI* crystal_obtained_icon;
	EntityUI* icon_RedCrystals;

	EntityUI* level2;
	EntityUI* level3;

	Material icon_redCrystal_material;

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

	EntityUI* playButton;
	EntityUI* quitButton;

	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class LoseStage : public Stage {

public:

	EntityUI* playAgainButton;

	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class PauseStage : public Stage {

public:

	EntityUI* continueButton;
	EntityUI* exitCourseButton;
	EntityUI* quitButton;

	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class TutorialStage : public Stage {

public:

	int Controller_background = 0;
	EntityUI* nextButton;

	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};

class CoursesSelectStage : public Stage {

public:

	EntityUI* TutorialButton;
	EntityUI* Level1Button;
	EntityUI* Level2Button;
	EntityUI* Level3Button;

	virtual void onEnter();
	virtual void onExit();

	void render();
	void update(double seconds_elapsed);
};