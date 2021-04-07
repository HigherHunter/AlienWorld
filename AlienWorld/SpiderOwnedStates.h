#ifndef SPIDER_OWNED_STATES_H
#define SPIDER_OWNED_STATES_H

#include "State.h"

class Spider;

class SpiderHide : public State<Spider>
{
private:

	SpiderHide() {}
	SpiderHide(const SpiderHide&);
	SpiderHide& operator=(const SpiderHide&);

public:

	// This is a singleton
	static SpiderHide* Instance();

	virtual void Enter(Spider* spider);

	virtual void Execute(Spider* spider);

	virtual void Exit(Spider* spider);
};

class SpiderChasePlayer : public State<Spider>
{
private:

	SpiderChasePlayer() {}
	SpiderChasePlayer(const SpiderChasePlayer&);
	SpiderChasePlayer& operator=(const SpiderChasePlayer&);

public:

	// This is a singleton
	static SpiderChasePlayer* Instance();

	virtual void Enter(Spider* spider);

	virtual void Execute(Spider* spider);

	virtual void Exit(Spider* spider);
};

class SpiderFlee : public State<Spider>
{
private:

	SpiderFlee() {}
	SpiderFlee(const SpiderFlee&);
	SpiderFlee& operator=(const SpiderFlee&);

public:

	// This is a singleton
	static SpiderFlee* Instance();

	virtual void Enter(Spider* spider);

	virtual void Execute(Spider* spider);

	virtual void Exit(Spider* spider);
};

#endif // !SPIDER_OWNED_STATES_H
