#include "Player.h"

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
Player::Player(
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

	Vehicle(
		id,
		world,
		position,
		rotation,
		velocity,
		mass,
		max_force,
		max_speed,
		max_turn_rate,
		scale)
{
	LoadAnimations();
}

//-------------------------------- Render -------------------------------------
//-----------------------------------------------------------------------------
void Player::Render()
{
	// Vector to north
	Vector2D vec(0, 1);

	// Angle between default vector and current looking vector - converted to degrees
	double angle = RadsToDegs(acos((vec.Dot(m_vLooking) / (vec.Length() * m_vLooking.Length()))));

	// Use Sign to determine which side is vector facing
	if (vec.Sign(m_vLooking) == -1)
	{
		angle = -angle;
	}

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Reset color
	glColor3f(1.0f, 1.0f, 1.0f); // white

	glBindTexture(GL_TEXTURE_2D, *it); // bind current frame

	if (alive)
	{
		if (it != runningAnim.end() - 1)
		{
			it++;
		}
		else
		{
			it = runningAnim.begin();
		}
	}
	else
	{
		if (it != deadAnim.end() - 1)
		{
			it++;
		}
	}

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

Bullet* Player::Shoot(unsigned int id)
{
	Bullet* pBullet = new Bullet(id,this->GetWorld(),
		Vector2D(this->GetPos().x, this->GetPos().y), //initial position
		0,        //start rotations
		Vector2D(0, 0),            //velocity
		1.0,			//mass
		1.0,			//max force
		15.0,            //max velocity
		1.0,			//max turn rate
		5.0);       //scale

	pBullet->SetBulletOwner(this);

	pBullet->MoveForward(Vec2DNormalize(GetLookingVec()));

	// Play sound
	SoundManager::instance().playFire();

	return pBullet;
}

void Player::GotHit(float damage)
{
	health -= damage;
	if (health <= 0)
	{
		health = 0;
		MarkDead();
	}
}

void Player::MarkDead()
{
	alive = false;
	it = deadAnim.begin();
	ColliderOff();
	deadTime = std::chrono::system_clock::now();
}

bool Player::ReadyToDelete()
{
	if (!alive)
	{
		currentTime = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = currentTime - deadTime;
		if (elapsed_seconds.count() >= 5)
		{
			return true;
		}
		return false;
	}
	return false;
}

void Player::LoadAnimations()
{
	for (int i = 0; i < 64; i++)
	{
		// Get images for running animation
		std::stringstream str_number;
		str_number << i + 1;
		std::string fileName = "Assets\\Sprites\\Player\\Running\\player_" + str_number.str() + ".png";
		runningAnim.push_back(LoadTexture((char*)fileName.c_str()));

		// Get images for dead animation
		fileName = "Assets\\Sprites\\Player\\Dead\\player_dead_" + str_number.str() + ".png";
		deadAnim.push_back(LoadTexture((char*)fileName.c_str()));
	}
	it = runningAnim.begin();
}