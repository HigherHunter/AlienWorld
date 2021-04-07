#include "Bullet.h"

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
Bullet::Bullet(
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
	m_personalID(id)
{
	box.SetMin(Vector2D(GetPos().x - GetBRadius(), GetPos().y - GetBRadius()));
	box.SetMax(Vector2D(GetPos().x + GetBRadius(), GetPos().y + GetBRadius()));
	LoadBulletImage();
}

//---------------------------- dtor -------------------------------------
//-----------------------------------------------------------------------
Bullet::~Bullet()
{
}

//------------------------------ Update ----------------------------------
//
//  Updates the vehicle's position from a series of steering behaviors
//------------------------------------------------------------------------
void Bullet::Update()
{
	box.SetMin(Vector2D(GetPos().x - GetBRadius(), GetPos().y - GetBRadius()));
	box.SetMax(Vector2D(GetPos().x + GetBRadius(), GetPos().y + GetBRadius()));
	//keep a record of its old position so we can update its cell later//in this method
	Vector2D oldPos = GetPos();

	Vector2D steeringForce;

	//calculate the combined force from each steering behavior in the //vehicle's list
	steeringForce = force;

	//Acceleration = Force/Mass
	Vector2D acceleration = steeringForce / m_dMass;

	//update velocity
	m_vVelocity += acceleration;

	//make sure vehicle does not exceed maximum velocity
	m_vVelocity.Truncate(m_dMaxSpeed);

	//update the position
	m_vPos += m_vVelocity;

	//update the heading if the vehicle has a non zero velocity
	if (m_vVelocity.LengthSq() > 0.00000001)
	{
		m_vHeading = Vec2DNormalize(m_vVelocity);

		m_vSide = m_vHeading.Perp();
	}
}

//-------------------------------- Render -------------------------------------
//-----------------------------------------------------------------------------
void Bullet::Render()
{
	// Vector to north
	Vector2D vec(0, 1);

	// Angle between default vector and current heading vector - converted to degrees
	double angle = RadsToDegs(acos((vec.Dot(m_vVelocity)) / (vec.Length() * m_vVelocity.Length())));

	// Use Sign to determine which side is vector facing
	if (vec.Sign(m_vVelocity) == -1)
	{
		angle = -angle;
	}

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Reset color
	glColor3f(1.0f, 1.0f, 1.0f); // white

	glBindTexture(GL_TEXTURE_2D, bullet_image); // bind current frame
	glPushMatrix();

	glTranslatef(GetPos().x, GetPos().y, 0);
	glRotatef(180, 0.0f, 0.0f, 1.0f); // Face image upwards like default vector
	glRotatef(angle, 0.0f, 0.0f, 1.0f); // Rotate image in place by given angle
	glTranslatef(-GetPos().x, -GetPos().y, 0);

	// Apply texture
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(GetPos().x - GetBRadius(), GetPos().y - GetBRadius());
	glTexCoord2f(1.0f, 0.0f); glVertex2f(GetPos().x + GetBRadius(), GetPos().y - GetBRadius());
	glTexCoord2f(1.0f, 1.0f); glVertex2f(GetPos().x + GetBRadius(), GetPos().y + GetBRadius());
	glTexCoord2f(0.0f, 1.0f); glVertex2f(GetPos().x - GetBRadius(), GetPos().y + GetBRadius());
	glEnd();

	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void Bullet::LoadBulletImage()
{
	bullet_image = LoadTexture((char*)"Assets\\Sprites\\Bullet\\Bullet.png");
}

void Bullet::MoveForward(Vector2D playerDirection)
{
	Vector2D desiredVelocity = Vec2DNormalize(playerDirection) * GetMaxSpeed();

	force = desiredVelocity;
}

bool Bullet::HasCollided(BaseGameEntity *v)
{
	bool collision = (box.GetMax().x < v->GetCollisionBox().GetMin().x) ||
		(v->GetCollisionBox().GetMax().x < box.GetMin().x) ||
		(box.GetMax().y < v->GetCollisionBox().GetMin().y) ||
		(v->GetCollisionBox().GetMax().y < box.GetMin().y);
	return !collision;
}

void Bullet::SetBulletOwner(Vehicle* v)
{
	bulletOwner = v;
}

