#include "AlienOwnedStates.h"
#include "Alien.h"

//-----------------AlienWander-----------------
AlienWander* AlienWander::Instance()
{
	static AlienWander instance;
	return &instance;
}

void AlienWander::Enter(Alien* alien)
{
	alien->GetSteering()->WanderOn();
}

void AlienWander::Execute(Alien* alien)
{
	if (alien->IsLeader())
	{
		if (alien->GetWorld()->IsPlayerWithinRadius(alien, 500))
			alien->GetFSM()->ChangeState(AlienChasePlayer::Instance());
	}
}

void AlienWander::Exit(Alien* alien)
{
	alien->GetSteering()->WanderOff();
}

//-----------------AlienOffsetPursuit-----------------
AlienOffsetPursuit* AlienOffsetPursuit::Instance()
{
	static AlienOffsetPursuit instance;
	return &instance;
}

void AlienOffsetPursuit::Enter(Alien* alien)
{
}

void AlienOffsetPursuit::Execute(Alien* alien)
{
	if (!alien->GetSteering()->GetTargetAgent1()->IsAlive())
	{
		alien->SetDamage(15);
		alien->GetFSM()->ChangeState(AlienChasePlayer::Instance());
	}
}

void AlienOffsetPursuit::Exit(Alien* alien)
{
	alien->GetSteering()->OffsetPursuitOff();
}

//----------------AlienChasePlayer------------------
AlienChasePlayer* AlienChasePlayer::Instance()
{
	static AlienChasePlayer instance;
	return &instance;
}

void AlienChasePlayer::Enter(Alien* alien)
{
	alien->GetSteering()->SeekOn();
	alien->GetSteering()->ObstacleAvoidanceOn();
}

void AlienChasePlayer::Execute(Alien* alien)
{
	// Is player within catch radius
	if (alien->GetWorld()->IsPlayerWithinRadius(alien, 150))
		alien->GetSteering()->SeparationOff();
}

void AlienChasePlayer::Exit(Alien* alien)
{
	alien->GetSteering()->SeekOff();
	alien->GetSteering()->ObstacleAvoidanceOff();
}