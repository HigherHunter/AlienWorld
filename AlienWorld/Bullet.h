#ifndef BULLET_H
#define BULLET_H

#include "MovingEntity.h"
#include "GameWorld.h"

class GameWorld;

// All bullets are stored in a map
typedef std::map<int, Bullet*> BulletMap;
static BulletMap Bullets;

class Bullet : public MovingEntity
{

private:

	//Unique ID for each bullet
	unsigned int m_personalID;

	// Pointer to world
	GameWorld *m_pWorld;

	// Static image of a bullet
	GLuint bullet_image;

	// Bullets collision box
	CollisionBox box;

	// Force applied to bullet
	Vector2D force;

	// How much damage bullet does
	float damage = 50;

	Vehicle* bulletOwner;

public:

	Bullet(
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

	~Bullet();

	void Update();

	void Render();

	void LoadBulletImage();

	// Bullet is able to move only forward when fired
	void MoveForward(Vector2D playerDirection);

	GameWorld*const GetWorld()const { return m_pWorld; }

	unsigned int GetID()const { return m_personalID; }

	float GetDamage()const { return damage; }
	void SetDamage(float new_damage) { damage = new_damage; }

	// Bullet can collide with any object
	bool HasCollided(BaseGameEntity *bse);

	Vehicle* GetBulletOwner() { return bulletOwner; }
	void SetBulletOwner(Vehicle* v);

};

#endif // !BULLET_H
