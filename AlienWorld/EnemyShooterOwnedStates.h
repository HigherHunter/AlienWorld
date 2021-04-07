#ifndef ENEMY_OWNED_STATES_H
#define ENEMY_OWNED_STATES_H

#include "State.h"

class EnemyShooter;

class EnemyShooterWander : public State<EnemyShooter>
{
private:

	EnemyShooterWander() {}
	EnemyShooterWander(const EnemyShooterWander&);
	EnemyShooterWander& operator=(const EnemyShooterWander&);

public:

	// This is a singleton
	static EnemyShooterWander* Instance();

	virtual void Enter(EnemyShooter* enemyShooter);

	virtual void Execute(EnemyShooter* enemyShooter);

	virtual void Exit(EnemyShooter* enemyShooter);
};

class EnemyShooterChasePlayer : public State<EnemyShooter>
{
private:

	EnemyShooterChasePlayer() {}
	EnemyShooterChasePlayer(const EnemyShooterChasePlayer&);
	EnemyShooterChasePlayer& operator=(const EnemyShooterChasePlayer&);

public:

	// This is a singleton
	static EnemyShooterChasePlayer* Instance();

	virtual void Enter(EnemyShooter* enemyShooter);

	virtual void Execute(EnemyShooter* enemyShooter);

	virtual void Exit(EnemyShooter* enemyShooter);
};

#endif // !ENEMY_OWNED_STATES_H
