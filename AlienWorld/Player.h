#ifndef PLAYER_H
#define PLAYER_H

#include "Vehicle.h"
#include "Bullet.h"

class Player : public Vehicle
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

	// Represents where player is looking
	Vector2D m_vLooking;

public:

	Player(
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

	void Render();

	Vector2D GetLookingVec()const { return m_vLooking; }
	void SetLookingVec(Vector2D new_vec) { m_vLooking = Vec2DNormalize(new_vec - GetPos()); }

	Bullet* Shoot(unsigned int id);

	void GotHit(float damage);

	void MarkDead();

	bool ReadyToDelete();

	void LoadAnimations();
};

#endif // !PLAYER_H
