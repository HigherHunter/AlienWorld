#include "SpiderOwnedStates.h"
#include "Spider.h"

//-----------------SpiderHide-----------------
SpiderHide* SpiderHide::Instance()
{
	static SpiderHide instance;
	return &instance;
}

void SpiderHide::Enter(Spider* spider)
{
	spider->GetSteering()->HideOn((Vehicle*)spider->GetWorld()->GetPlayer());
	spider->GetSteering()->ObstacleAvoidanceOff();
}

void SpiderHide::Execute(Spider* spider)
{
	if (spider->GetWorld()->IsPlayerWithinRadius(spider, 300))
		spider->GetFSM()->ChangeState(SpiderFlee::Instance());
}

void SpiderHide::Exit(Spider* spider)
{
	spider->GetSteering()->HideOff();
}

//----------------ChasePlayer------------------
SpiderChasePlayer* SpiderChasePlayer::Instance()
{
	static SpiderChasePlayer instance;
	return &instance;
}

void SpiderChasePlayer::Enter(Spider* spider)
{
	spider->SetMaxSpeed(4.0f);
	spider->GetSteering()->SeekOn();
	spider->GetSteering()->ObstacleAvoidanceOn();
}

void SpiderChasePlayer::Execute(Spider* spider)
{
	// Is player within catch radius
	if (spider->GetWorld()->IsPlayerWithinRadius(spider, 150))
		spider->GetSteering()->SeparationOff();
}

void SpiderChasePlayer::Exit(Spider* spider)
{
	spider->GetSteering()->SeekOff();
}

//----------------SpiderFlee------------------
SpiderFlee* SpiderFlee::Instance()
{
	static SpiderFlee instance;
	return &instance;
}

void SpiderFlee::Enter(Spider* spider)
{
	spider->SetMaxSpeed(5.0f);
	spider->GetSteering()->ObstacleAvoidanceOn();
	spider->GetSteering()->FleeOn();
}

void SpiderFlee::Execute(Spider* spider)
{
	if (!spider->GetWorld()->IsPlayerWithinRadius(spider, 400))
		spider->GetFSM()->ChangeState(SpiderChasePlayer::Instance());
}

void SpiderFlee::Exit(Spider* spider)
{
	spider->GetSteering()->FleeOff();
}