#ifndef ENEMY_SHOOTER_H
#define ENEMY_SHOOTER_H

#include "Vehicle.h"
#include "Bullet.h"
#include "EnemyShooterOwnedStates.h"
#include "StateMachine.h"

class EnemyShooter : public Vehicle
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
	StateMachine<EnemyShooter>* m_pStateMachine;

	// Collor
	GLfloat red, green, blue;

	// Points awarding to player
	int points;

public:

	EnemyShooter(
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

	void LoadAnimations();

	void SetColor(GLfloat r, GLfloat g, GLfloat b) { red = r, green = g, blue = b; }

	int GetPoints() { return points; }

	StateMachine<EnemyShooter>* GetFSM()const { return m_pStateMachine; }

	Bullet* Shoot(unsigned int id);
};

#endif // !ENEMY_SHOOTER_H
