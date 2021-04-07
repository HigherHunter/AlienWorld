#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <fmod\fmod.hpp>

class SoundManager
{
private:

	FMOD::System *system;
	FMOD::Sound *alienAttack, *alienDead, *spiderAttack, *spiderDead, *zombieAttack, *zombieDead;
	FMOD::Sound *hit, *fire, *background;
	FMOD::Channel *channel = 0;
	FMOD_RESULT result;
	unsigned int version;
	void *extradriverdata = 0;

	SoundManager();

public:

	SoundManager(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;

	// Singleton
	static SoundManager& instance()
	{
		static SoundManager INSTANCE;
		return INSTANCE;
	}

	~SoundManager();

	void playAlienAttack();
	void playAlienDead();
	void playSpiderAttack();
	void playSpiderDead();
	void playZombieAttack();
	void playZombieDead();
	void playHit();
	void playFire();

};

#endif // !SOUND_MANAGER_H
