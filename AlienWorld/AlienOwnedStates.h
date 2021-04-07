#ifndef ALIEN_OWNED_STATES_H
#define ALIEN_OWNED_STATES_H

#include "State.h"

class Alien;

class AlienWander : public State<Alien>
{
private:

	AlienWander() {}
	AlienWander(const AlienWander&);
	AlienWander& operator=(const AlienWander&);

public:

	// This is a singleton
	static AlienWander* Instance();

	virtual void Enter(Alien* alien);

	virtual void Execute(Alien* alien);

	virtual void Exit(Alien* alien);
};

class AlienOffsetPursuit : public State<Alien>
{
private:

	AlienOffsetPursuit() {}
	AlienOffsetPursuit(const AlienOffsetPursuit&);
	AlienOffsetPursuit& operator=(const AlienOffsetPursuit&);

public:

	// This is a singleton
	static AlienOffsetPursuit* Instance();

	virtual void Enter(Alien* alien);

	virtual void Execute(Alien* alien);

	virtual void Exit(Alien* alien);
};

class AlienChasePlayer : public State<Alien>
{
private:

	AlienChasePlayer() {}
	AlienChasePlayer(const AlienChasePlayer&);
	AlienChasePlayer& operator=(const AlienChasePlayer&);

public:

	// This is a singleton
	static AlienChasePlayer* Instance();

	virtual void Enter(Alien* alien);

	virtual void Execute(Alien* alien);

	virtual void Exit(Alien* alien);
};

#endif // !ALIEN_OWNED_STATES_H
