#include "EnemyShooterOwnedStates.h"
#include "EnemyShooter.h"

//-----------------EnemyShooterWander-----------------
EnemyShooterWander* EnemyShooterWander::Instance()
{
	static EnemyShooterWander instance;
	return &instance;
}

void EnemyShooterWander::Enter(EnemyShooter* enemyShooter)
{
	enemyShooter->GetSteering()->WanderOn();
}

void EnemyShooterWander::Execute(EnemyShooter* enemyShooter)
{
	if (enemyShooter->GetWorld()->IsPlayerWithinRadius(enemyShooter, 500))
		enemyShooter->GetFSM()->ChangeState(EnemyShooterChasePlayer::Instance());
}

void EnemyShooterWander::Exit(EnemyShooter* enemyShooter)
{
	enemyShooter->GetSteering()->WanderOff();
}

//----------------EnemyShooterChasePlayer------------------
EnemyShooterChasePlayer* EnemyShooterChasePlayer::Instance()
{
	static EnemyShooterChasePlayer instance;
	return &instance;
}

void EnemyShooterChasePlayer::Enter(EnemyShooter* enemyShooter)
{
	enemyShooter->GetSteering()->SeekOn();
	enemyShooter->GetSteering()->ObstacleAvoidanceOn();
}

void EnemyShooterChasePlayer::Execute(EnemyShooter* enemyShooter)
{
	if (enemyShooter->IsAlive())
		enemyShooter->GetWorld()->CreateEnemyShooterBullet(enemyShooter);
}

void EnemyShooterChasePlayer::Exit(EnemyShooter* enemyShooter)
{
	enemyShooter->GetSteering()->SeekOff();
	enemyShooter->GetSteering()->ObstacleAvoidanceOff();
}