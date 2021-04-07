#ifndef ALIEN_H
#define ALIEN_H

#include "Vehicle.h"
#include "AlienOwnedStates.h"
#include "StateMachine.h"

class Alien : public Vehicle
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
	StateMachine<Alien>* m_pStateMachine;

	// Collor
	GLfloat red, green, blue;

	// Is alien leader
	bool leader;

	// Points awarding to player
	int points;

public:

	Alien(
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

	void SetColor(GLfloat r, GLfloat g, GLfloat b) { red = r, green = g, blue = b; }

	bool IsLeader()const { return leader; }
	void SetAsLeader() { leader = true; }

	int GetPoints() { return points; }

	StateMachine<Alien>* GetFSM()const { return m_pStateMachine; }
};

#endif // !ALIEN_H
