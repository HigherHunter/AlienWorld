#include "ZombieOwnedStates.h"
#include "Zombie.h"

//-----------------ZombiePatrol-----------------
ZombiePatrol* ZombiePatrol::Instance()
{
	static ZombiePatrol instance;
	return &instance;
}

void ZombiePatrol::Enter(Zombie* zombie)
{
	zombie->GetSteering()->PatrolOn();
}

void ZombiePatrol::Execute(Zombie* zombie)
{
	if (zombie->GetWorld()->IsPlayerWithinRadius(zombie, 300))
		zombie->GetFSM()->ChangeState(ZombieChasePlayer::Instance());
}

void ZombiePatrol::Exit(Zombie* zombie)
{
	zombie->GetSteering()->PatrolOff();
}

//----------------ChasePlayer------------------
ZombieChasePlayer* ZombieChasePlayer::Instance()
{
	static ZombieChasePlayer instance;
	return &instance;
}

void ZombieChasePlayer::Enter(Zombie* zombie)
{
	zombie->GetSteering()->SeekOn();
	zombie->GetSteering()->ObstacleAvoidanceOn();
}

void ZombieChasePlayer::Execute(Zombie* zombie)
{
	// Is player within catch radius
	if (zombie->GetWorld()->IsPlayerWithinRadius(zombie, 150))
		zombie->GetSteering()->SeparationOff();
}

void ZombieChasePlayer::Exit(Zombie* zombie)
{
	zombie->GetSteering()->SeekOff();
	zombie->GetSteering()->ObstacleAvoidanceOff();
}