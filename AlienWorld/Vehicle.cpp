#include "Vehicle.h"

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
Vehicle::Vehicle(
	unsigned int id,
	GameWorld *world,
	Vector2D position,
	double rotation,
	Vector2D velocity,
	double mass,
	double max_force,
	double max_speed,
	double max_turn_rate,
	double scale) :

	MovingEntity(
		position,
		scale,
		velocity,
		max_speed,
		Vector2D(sin(rotation), -cos(rotation)),
		mass,
		Vector2D(scale, scale),
		max_turn_rate,
		max_force),

	m_pWorld(world),
	m_PersonalID(id)
{
	m_pSteering = new SteeringBehavior(this);
	box.SetMin(Vector2D(GetPos().x - GetBRadius() / 2, GetPos().y - GetBRadius() / 2));
	box.SetMax(Vector2D(GetPos().x + GetBRadius() / 2, GetPos().y + GetBRadius() / 2));
}

//---------------------------- dtor -------------------------------------
//-----------------------------------------------------------------------
Vehicle::~Vehicle()
{
	delete m_pSteering;
}

//------------------------------ Update ----------------------------------
//
//  Updates the vehicle's position from a series of steering behaviors
//------------------------------------------------------------------------
void Vehicle::Update()
{
	if (alive)
	{
		// Keep a record of its old position so we can update its cell later in this method
		Vector2D oldPos = GetPos();

		Vector2D steeringForce;

		// Calculate the combined force from each steering behavior in the  vehicle's list
		steeringForce = m_pSteering->Calculate();

		// Acceleration = Force/Mass
		Vector2D acceleration = steeringForce / m_dMass;

		// Update velocity
		m_vVelocity += acceleration;

		// Make sure vehicle does not exceed maximum velocity
		m_vVelocity.Truncate(m_dMaxSpeed);

		// Update the position
		m_vPos += m_vVelocity;

		// Update the heading if the vehicle has a non zero velocity
		if (m_vVelocity.LengthSq() > 0.00000001)
		{
			m_vHeading = Vec2DNormalize(m_vVelocity);

			m_vSide = m_vHeading.Perp();
		}

		// Update collision box position
		box.SetMin(Vector2D(GetPos().x - GetBRadius() / 2, GetPos().y - GetBRadius() / 2));
		box.SetMax(Vector2D(GetPos().x + GetBRadius() / 2, GetPos().y + GetBRadius() / 2));
	}
}

bool Vehicle::ReadyToHit()
{
	currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = currentTime - hitTime;
	if ((int)elapsed_seconds.count() >= 1)
	{
		hitTime = std::chrono::system_clock::now();
		return true;
	}
	return false;
}

bool Vehicle::HasCollided(BaseGameEntity *v)
{
	if (alive)
	{
		if (ReadyToHit())
		{
			if (m_vPos.Distance(v->GetPos()) < m_dBoundingRadius * 1.5)
			{
				bool collision = (box.GetMax().x < v->GetCollisionBox().GetMin().x) ||
					(v->GetCollisionBox().GetMax().x < box.GetMin().x) ||
					(box.GetMax().y < v->GetCollisionBox().GetMin().y) ||
					(v->GetCollisionBox().GetMax().y < box.GetMin().y);
				return !collision;
			}
		}
	}
	return false;
}