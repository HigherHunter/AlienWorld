#ifndef ZOMBIE_OWNED_STATES_H
#define ZOMBIE_OWNED_STATES_H

#include "State.h"

class Zombie;

class ZombiePatrol : public State<Zombie>
{
private:

	ZombiePatrol() {}
	ZombiePatrol(const ZombiePatrol&);
	ZombiePatrol& operator=(const ZombiePatrol&);

public:

	// This is a singleton
	static ZombiePatrol* Instance();

	virtual void Enter(Zombie* zombie);

	virtual void Execute(Zombie* zombie);

	virtual void Exit(Zombie* zombie);
};

class ZombieChasePlayer : public State<Zombie>
{
private:

	ZombieChasePlayer() {}
	ZombieChasePlayer(const ZombieChasePlayer&);
	ZombieChasePlayer& operator=(const ZombieChasePlayer&);

public:

	// This is a singleton
	static ZombieChasePlayer* Instance();

	virtual void Enter(Zombie* zombie);

	virtual void Execute(Zombie* zombie);

	virtual void Exit(Zombie* zombie);
};

#endif // !ZOMBIE_OWNED_STATES_H
