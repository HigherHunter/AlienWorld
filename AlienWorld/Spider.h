#ifndef SPIDER_H
#define SPIDER_H

#include "Vehicle.h"
#include "SpiderOwnedStates.h"
#include "StateMachine.h"

class Spider : public Vehicle
{
private:

	// Holds images that make running animation
	std::vector<GLuint> runningAnim;
	// Holds images that make dead animation
	std::vector<GLuint> deadAnim;

	// iterator that goes through lists
	std::vector<GLuint>::iterator it;

	// time of death
	std::chrono::system_clock::time_point deadTime;
	// current time
	std::chrono::system_clock::time_point currentTime;

	// An instance of the state machine class
	StateMachine<Spider>* m_pStateMachine;

	// Points awarding to player
	int points;

public:

	Spider(
		unsigned int id,
		GameWorld *world,
		Vector2D position,
		double rotation,
		Vector2D velocity,
		double mass,
		double max_force,
		double max_speed,
		double max_turn_rate,
		double scale);

	void Update();

	void Render();

	void GotHit(float damage);

	void MarkDead();

	bool ReadyToDelete();

	void MakeAttackSound();

	void LoadAnimations();

	int GetPoints() { return points; }

	StateMachine<Spider>* GetFSM()const { return m_pStateMachine; }
};

#endif // !SPIDER_H
