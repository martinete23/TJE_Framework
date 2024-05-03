#include "stage.h"

#include "world.h"

void IntroStage::onEnter()
{
}

void IntroStage::onExit()
{
}

void IntroStage::render()
{
	World::instance->render();
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
