#pragma once

#include "input.h"
#include "world.h"
#include "graphics/shader.h"

class World;

enum eStages {
	INTRO,
	PLAY,
	WIN,
	LOSE,
	STAGES_SIZE
};

class Stage {
public:
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

	void render(Shader* shader);
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
