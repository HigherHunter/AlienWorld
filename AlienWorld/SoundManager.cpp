#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager()
{
	// constructor
	result = FMOD::System_Create(&system);
	result = system->getVersion(&version);

	if (version < FMOD_VERSION)
	{
		// Error
	}

	result = system->init(4, FMOD_INIT_NORMAL, extradriverdata);

	result = system->createSound("Assets\\Audio\\alienAttack.mp3", FMOD_DEFAULT, 0, &alienAttack);
	result = system->createSound("Assets\\Audio\\alienDead.mp3", FMOD_DEFAULT, 0, &alienDead);
	result = system->createSound("Assets\\Audio\\spiderAttack.mp3", FMOD_DEFAULT, 0, &spiderAttack);
	result = system->createSound("Assets\\Audio\\spiderDead.mp3", FMOD_DEFAULT, 0, &spiderDead);
	result = system->createSound("Assets\\Audio\\zombieAttack.mp3", FMOD_DEFAULT, 0, &zombieAttack);
	result = system->createSound("Assets\\Audio\\zombieDead.mp3", FMOD_DEFAULT, 0, &zombieDead);
	result = system->createSound("Assets\\Audio\\hit.mp3", FMOD_DEFAULT, 0, &hit);
	result = system->createSound("Assets\\Audio\\fire.mp3", FMOD_DEFAULT, 0, &fire);
	result = background->setMode(FMOD_LOOP_NORMAL);
}

SoundManager::~SoundManager()
{
	// destructor
	result = alienAttack->release();
	result = alienDead->release();
	result = spiderAttack->release();
	result = spiderDead->release();
	result = hit->release();
	result = fire->release();
	result = background->release();

	result = system->close();
	result = system->release();
}

void SoundManager::playAlienAttack()
{
	result = system->playSound(alienAttack, 0, false, &channel);
}

void SoundManager::playAlienDead()
{
	result = system->playSound(alienDead, 0, false, &channel);
}

void SoundManager::playSpiderAttack()
{
	result = system->playSound(spiderAttack, 0, false, &channel);
}

void SoundManager::playSpiderDead()
{
	result = system->playSound(spiderDead, 0, false, &channel);
}

void SoundManager::playZombieAttack()
{
	result = system->playSound(zombieAttack, 0, false, &channel);
}

void SoundManager::playZombieDead()
{
	result = system->playSound(zombieDead, 0, false, &channel);
}

void SoundManager::playHit()
{
	result = system->playSound(hit, 0, false, &channel);
}

void SoundManager::playFire()
{
	result = system->playSound(fire, 0, false, &channel);
}