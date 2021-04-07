#ifndef VEHICLE_H
#define VEHICLE_H

#include "Obstacle.h"
#include "MovingEntity.h"
#include "SteeringBehaviors.h"
#include "GameWorld.h"

class GameWorld;
class SteeringBehavior;

// All vehicles are stored in a map
typedef std::map<int, Vehicle*> VehicleMap;
static VehicleMap Vehicles;

class Vehicle : public MovingEntity
{

private:

	// A pointer to the world data. So a vehicle can access any obstacle, path, wall or agent data
	GameWorld* m_pWorld;

	// The steering behavior class
	SteeringBehavior* m_pSteering;

	//disallow the copying of Vehicle types
	Vehicle(const Vehicle&);
	Vehicle& operator=(const Vehicle&);

	// Hit time
	std::chrono::system_clock::time_point hitTime = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point currentTime;

protected:

	float maxHealth = 100;

	float health = maxHealth;

	float damage = 5;

	// Unique ID for each entity
	unsigned int m_PersonalID;

	bool alive = true;

	// looking angle
	double angle;

public:

	bool probableJitter = false;

	Vehicle(
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

	~Vehicle();

	// Updates the vehicle's position and orientation
	virtual void Update();

	void Render() {};

	virtual void GotHit(float damage) {};

	// Is ready to attack
	bool ReadyToHit();

	virtual void MakeAttackSound() {};

	// Has it collided with another object
	bool HasCollided(BaseGameEntity *bse);

	virtual void MarkDead() {};

	// Is ready to be deleted
	virtual bool ReadyToDelete() { return false; }

	virtual int GetPoints() { return 0; }

	SteeringBehavior*const GetSteering()const { return m_pSteering; }
	GameWorld*const GetWorld()const { return m_pWorld; }

	unsigned int GetID()const { return m_PersonalID; }
	float GetMaxHealth()const { return maxHealth; }
	void SetMaxHealth(float new_max_health) { maxHealth = new_max_health; }
	float GetHealth()const { return health; }
	void SetHealth(float new_health) { health = new_health; }
	float HealthPercentLost()const { return ((maxHealth - health) / maxHealth * 100); }
	float GetDamage()const { return damage; }
	void SetDamage(float new_damage) { damage = new_damage; }
	bool IsAlive()const { return alive; }
};

#endif // !VEHICLE_H