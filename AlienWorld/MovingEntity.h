#ifndef MOVING_ENTITY
#define MOVING_ENTITY

#include "BaseGameEntity.h"
#include "SoundManager.h"

class MovingEntity : public BaseGameEntity
{
protected:

	Vector2D m_vVelocity;

	// A normalized vector pointing in the direction the entity is heading. 
	Vector2D m_vHeading;

	// A vector perpendicular to the heading vector
	Vector2D m_vSide;

	// Objects mass
	double m_dMass;

	// The maximum speed this entity may travel at.
	double m_dMaxSpeed;

	// The maximum force this entity can produce to power itself think rockets and thrust
	double m_dMaxForce;

	// The maximum rate (radians per second)this vehicle can rotate         
	double m_dMaxTurnRate;

public:

	MovingEntity(Vector2D position,
		double radius,
		Vector2D velocity,
		double max_speed,
		Vector2D heading,
		double mass,
		Vector2D scale,
		double turn_rate,
		double max_force) :BaseGameEntity(0, position, radius),
		m_vHeading(heading),
		m_vVelocity(velocity),
		m_dMass(mass),
		m_vSide(m_vHeading.Perp()),
		m_dMaxSpeed(max_speed),
		m_dMaxTurnRate(turn_rate),
		m_dMaxForce(max_force)
	{
		m_vScale = scale;
	}

	virtual ~MovingEntity() {}

	Vector2D GetVelocity()const { return m_vVelocity; }
	void SetVelocity(const Vector2D& NewVel) { m_vVelocity = NewVel; }

	double GetMass()const { return m_dMass; }

	Vector2D GetSide()const { return m_vSide; }

	double GetMaxSpeed()const { return m_dMaxSpeed; }
	void SetMaxSpeed(double new_speed) { m_dMaxSpeed = new_speed; }

	double GetMaxForce()const { return m_dMaxForce; }
	void SetMaxForce(double mf) { m_dMaxForce = mf; }

	bool IsSpeedMaxedOut()const { return m_dMaxSpeed * m_dMaxSpeed >= m_vVelocity.LengthSq(); }
	double GetSpeed()const { return m_vVelocity.Length(); }
	double GetSpeedSq()const { return m_vVelocity.LengthSq(); }

	Vector2D GetHeading()const { return m_vHeading; }
	void SetHeading(Vector2D new_heading);

	double GetMaxTurnRate()const { return m_dMaxTurnRate; }
	void SetMaxTurnRate(double val) { m_dMaxTurnRate = val; }

};

//------------------------- SetHeading ----------------------------------------
//
//  first checks that the given heading is not a vector of zero length. If the
//  new heading is valid this function sets the entity's heading and side 
//  vectors accordingly
//-----------------------------------------------------------------------------
inline void MovingEntity::SetHeading(Vector2D new_heading)
{
	assert((new_heading.LengthSq() - 1.0) < 0.00001);

	m_vHeading = new_heading;

	// The side vector must always be perpendicular to the heading
	m_vSide = m_vHeading.Perp();
}

#endif // !MOVING_ENTITY