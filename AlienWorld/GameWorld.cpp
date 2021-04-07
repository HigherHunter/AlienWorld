#include "GameWorld.h"
#include "Obstacle.h"
#include "SteeringBehaviors.h"
#include "Alien.h"
#include "Zombie.h"
#include "Spider.h"
#include "EnemyShooter.h"
#include "Player.h"

VehicleMap::iterator vehicleIt;
VehicleMap::iterator vehicleIt2;
BulletMap::iterator bulletIt;

std::vector<NODE> pathL;
Path *pathCreator;

// Pointers
Obstacle* pObstacle;
Bullet* b;
Vehicle* v;
Alien* pAlien;
Spider* pSpider;
Zombie* pZombie;
EnemyShooter* pEnemyShooter;

// game map for path and A*
std::map<std::pair<int, int>, int> gameMap;

int timeDifference = 0;

//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
GameWorld::GameWorld(int cx, int cy, unsigned int level) :

	m_WindowSizeX(cx),
	m_WindowSizeY(cy),
	currentLevel(level)
{
	LevelDesignInitialization();

	noOfEnemies = Vehicles.size() - 1;

	killTime = std::chrono::high_resolution_clock::now();
}

void GameWorld::CreatePlayerBullet()
{
	if (staticPlayer->ReadyToHit())
	{
		Bullets[bulletTracker] = staticPlayer->Shoot(bulletTracker);
		bulletTracker++;
	}
}

void GameWorld::CreateEnemyShooterBullet(EnemyShooter* shooter)
{
	if (shooter->ReadyToHit())
	{
		Bullets[bulletTracker] = shooter->Shoot(bulletTracker);
		bulletTracker++;
	}
}

void GameWorld::RemoveBullet(Bullet* b)
{
	Bullets.erase(b->GetID());
	bulletTracker--;
	delete b;
}

void GameWorld::RemoveAgent(Vehicle* v)
{
	Vehicles.erase(v->GetID());
	vehicleTracker--;
	delete v;
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
GameWorld::~GameWorld()
{
	// Delete evrything

	Vehicles.clear();
	Bullets.clear();

	for (unsigned int ob = 0; ob < m_Obstacles.size(); ++ob)
	{
		delete m_Obstacles[ob];
	}

	if (m_pPath != NULL)
		delete m_pPath;
}

//----------------------------- Update -----------------------------------
//------------------------------------------------------------------------
void GameWorld::Update()
{
	// get current time
	currentTime = std::chrono::high_resolution_clock::now();

	// clear destruction lists
	m_destroyb.clear();
	m_destroyv.clear();

	if (killCount == noOfEnemies) // minus player
		gameWon = true;

	// Fixed update for physics (update moving vehicles)
	if (FixedUpdateTime())
	{
		// Update player
		staticPlayer->Update();
		if (!staticPlayer->IsAlive())
			if (staticPlayer->ReadyToDelete())
			{
				gameOver = true;
			}
		// Update vehicles
		for (vehicleIt = std::next(Vehicles.begin(), 1); vehicleIt != Vehicles.end(); ++vehicleIt)
		{
			v = (*vehicleIt).second;

			v->GetSteering()->SetTarget(staticPlayer->GetPos());

			v->Update();

			if (!v->IsAlive())
				if (v->ReadyToDelete())
					m_destroyv.push_back(v);

			if (staticPlayer->IsColliderActive())
				if (v->HasCollided(staticPlayer))
				{
					staticPlayer->GotHit(v->GetDamage());
					v->MakeAttackSound();
				}
		}
		// Update bullets
		for (bulletIt = Bullets.begin(); bulletIt != Bullets.end(); ++bulletIt)
		{
			b = (*bulletIt).second;
			b->Update();

			for (vehicleIt = Vehicles.begin(); vehicleIt != Vehicles.end(); ++vehicleIt)
			{
				if (b->GetBulletOwner() != (*vehicleIt).second)
				{
					v = (*vehicleIt).second;
					if (v->IsColliderActive())
					{
						if (b->HasCollided(v))
						{
							v->GotHit(b->GetDamage());
							if (!v->IsAlive())
							{
								killCount++;
								killScore += v->GetPoints();
								killTime = std::chrono::high_resolution_clock::now();
							}
							m_destroyb.push_back(b);
							break;
						}
					}
				}
			}
			for (unsigned int i = 0; i < m_Obstacles.size(); ++i)
			{
				if (b->HasCollided(m_Obstacles[i]))
				{
					m_destroyb.push_back(b);
					break;
				}
			}
			if (b->GetPos().x < 0 || b->GetPos().x > MAP_WIDTH || b->GetPos().y < 0 || b->GetPos().y > MAP_HEIGHT)
			{
				m_destroyb.push_back(b);
			}
		}
	}

	// Render dead vehicles first
	for (vehicleIt = std::next(Vehicles.begin(), 1); vehicleIt != Vehicles.end(); ++vehicleIt)
	{
		v = (*vehicleIt).second;
		if (!v->IsAlive())
			v->Render();
	}

	// Render help indicator if needed
	if (!IsGamePaused())
	{
		if (ShouldShowIndicator())
		{
			Vehicle* closestEnemy = FindClosestEnemy();

			Vector2D arrow = staticPlayer->GetPos() + Vec2DNormalize(closestEnemy->GetPos() - staticPlayer->GetPos()) * 100;

			drawArrow(staticPlayer->GetPos().x, staticPlayer->GetPos().y, arrow.x, arrow.y, 50);
		}
	}

	// Render bullets
	for (bulletIt = Bullets.begin(); bulletIt != Bullets.end(); ++bulletIt)
	{
		b = (*bulletIt).second;
		b->Render();
	}

	// Render player
	staticPlayer->Render();

	// Render alive vehicles
	for (vehicleIt = std::next(Vehicles.begin(), 1); vehicleIt != Vehicles.end(); ++vehicleIt)
	{
		v = (*vehicleIt).second;
		if (v->IsAlive())
			v->Render();
	}

	// Render obstacles
	for (unsigned int i = 0; i < m_Obstacles.size(); ++i)
	{
		m_Obstacles[i]->Render();
	}

	// Delete everything marked for removal
	for (bDestroyIt = m_destroyb.begin(); bDestroyIt != m_destroyb.end(); ++bDestroyIt)
	{
		if (Bullets.size() == 0) break;
		RemoveBullet(*bDestroyIt);
	}

	for (vDestoryIt = m_destroyv.begin(); vDestoryIt != m_destroyv.end(); ++vDestoryIt)
	{
		if (Vehicles.size() == 0) break;
		RemoveAgent(*vDestoryIt);
	}
}

Vehicle* GameWorld::GetAgentAt(unsigned int key)
{
	return Vehicles.at(key);
}

bool GameWorld::FixedUpdateTime()
{
	std::chrono::high_resolution_clock::time_point tempTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = tempTime - gameTime;
	if ((int)(elapsed_seconds.count() * 1000) >= 25)
	{
		gameTime = std::chrono::high_resolution_clock::now();
		return true;
	}
	return false;
}

bool GameWorld::ShouldShowIndicator()
{
	std::chrono::high_resolution_clock::time_point tempTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = tempTime - killTime;
	if ((int)(elapsed_seconds.count()) >= 30)
	{
		return true;
	}
	return false;
}

void GameWorld::TagVehiclesWithinViewrange(Vehicle* pVehicle, double range)
{
	m_Neighbours.clear();
	// Iterate through all entities checking for range
	for (vehicleIt2 = std::next(Vehicles.begin(), 1); vehicleIt2 != Vehicles.end(); ++vehicleIt2)
	{
		// Is alive?
		if ((*vehicleIt2).second->IsAlive())
		{
			Vector2D to = (*vehicleIt2).second->GetPos() - pVehicle->GetPos();

			// The bounding radius of the other is taken into account by adding it to the range
			double radius = range + (*vehicleIt2).second->GetBRadius();

			// If entitiy is within range, tag for further consideration
			// (Working in distance-squared space to avoid sqrts)
			if ((*vehicleIt2).second != pVehicle && to.LengthSq() < range*range)
			{
				m_Neighbours.push_back((*vehicleIt2).second);
			}
		}
	}
}

Vehicle* GameWorld::FindClosestEnemy()
{
	double distance = MAP_WIDTH;
	Vehicle* closestEnemy = staticPlayer;
	// Iterate through all entities and find closest
	for (vehicleIt = std::next(Vehicles.begin(), 1); vehicleIt != Vehicles.end(); ++vehicleIt)
	{
		// Is alive?
		if ((*vehicleIt).second->IsAlive())
		{
			double tempDistance = Vec2DDistance((*vehicleIt).second->GetPos(), staticPlayer->GetPos());
			if (tempDistance < distance)
			{
				distance = tempDistance;
				closestEnemy = (*vehicleIt).second;
			}
		}
	}
	return closestEnemy;
}

void GameWorld::GamePausedOn()
{
	gamePaused = true;
	std::chrono::duration<double> elapsed_seconds = std::chrono::high_resolution_clock::now() - killTime;
	timeDifference = (int)elapsed_seconds.count();
}

void GameWorld::GamePausedOff()
{
	gamePaused = false;
	killTime = std::chrono::high_resolution_clock::now() - std::chrono::seconds(timeDifference);
}

bool GameWorld::IsPlayerWithinRadius(Vehicle* pVehicle, double range)
{
	if (Vec2DDistanceSq(pVehicle->GetPos(), staticPlayer->GetPos()) < range * range)
		return true;
	return false;
}

void GameWorld::PathInitialization()
{
	pathCreator = new Path();

	m_pPath = new Path();

	pathCreator->SetupMap(gameMap);
	pathCreator->Create();
	pathL = pathCreator->BackTrack();

	//set path according to map
	for (unsigned int i = pathL.size() - 2; i > 0; i--)
	{
		m_pPath->AddWayPoint(Vector2D((pathL[i].x - 1) * 64 + 32, (pathL[i].y - 1) * 64 + 32));
	}

	//clean the path list
	pathL.erase(pathL.begin(), pathL.begin() + pathL.size());
	m_pPath->LoopOff();
	delete pathCreator;
}

void GameWorld::LevelDesignInitialization()
{
	int randX = 0, randY = 0;
	int tempRandX = 0, tempRandY = 0;
	Vector2D position;

	if (currentLevel == 1)
	{
		// Obstacles must be made before path
		// hight 33, width 60
		//clear all the matrix with the ground tile
		for (int i = 1; i <= TILE_COUNT_HEIGHT; i++)
		{
			for (int j = 1; j <= TILE_COUNT_WIDTH; j++)
			{
				gameMap[{i, j}] = 0;
			}
		}

		randX = select_randomly(1, TILE_COUNT_WIDTH);
		randY = select_randomly(1, TILE_COUNT_HEIGHT);

		// Make 15 obstacles
		for (unsigned int i = 0; i < 15; i++)
		{
			while (gameMap[{randX, randY}] != 0)
			{
				randX = select_randomly(1, TILE_COUNT_WIDTH);
				randY = select_randomly(1, TILE_COUNT_HEIGHT);
			}

			pObstacle = new Obstacle(Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32), 32); // x = 3, y = 1, +64 to move
			m_Obstacles.push_back(pObstacle);

			gameMap[{randX, randY}] = 1;
		}


		//----------------Add Player---------------------
		randX = select_randomly(1, TILE_COUNT_WIDTH);
		randY = select_randomly(1, TILE_COUNT_HEIGHT);

		while (gameMap[{randX, randY}] != 0)
		{
			randX = select_randomly(1, TILE_COUNT_WIDTH);
			randY = select_randomly(1, TILE_COUNT_HEIGHT);
		}

		position = Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32);

		staticPlayer = new Player(
			vehicleTracker,
			this,
			position, //initial position
			RandFloat()*TwoPi, //start rotations
			Vector2D(0, 0),    //velocity
			1.0,			//mass
			1.0,			//max force
			4.0,            //max velocity
			1.0,			//max turn rate
			32);			//scale

		staticPlayer->GetSteering()->SetTarget(staticPlayer->GetPos());
		staticPlayer->GetSteering()->ArriveOn();
		staticPlayer->GetSteering()->ObstacleAvoidanceOn();
		Vehicles[vehicleTracker] = staticPlayer;
		vehicleTracker++;

		// [x][y]
		gameMap[{randX, randY}] = 1; // player position

		// Make 10 zombies
		for (int i = 0; i < 10; i++)
		{
			randX = select_randomly(1, TILE_COUNT_WIDTH - 1);
			randY = select_randomly(1, TILE_COUNT_HEIGHT - 1);

			while (gameMap[{randX, randY}] != 0)
			{
				randX = select_randomly(1, TILE_COUNT_WIDTH - 1);
				randY = select_randomly(1, TILE_COUNT_HEIGHT - 1);
			}

			position = Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32);

			// [x][y]
			gameMap[{randX, randY}] = 8; // start

			tempRandX = select_randomly(1, TILE_COUNT_WIDTH - 1);
			tempRandY = select_randomly(1, TILE_COUNT_HEIGHT - 1);

			while (gameMap[{tempRandX, tempRandY}] != 0)
			{
				tempRandX = select_randomly(1, TILE_COUNT_WIDTH - 1);
				tempRandY = select_randomly(1, TILE_COUNT_HEIGHT - 1);
			}

			gameMap[{tempRandX, tempRandY}] = 9; // goal

			// Make A* path
			PathInitialization();

			gameMap[{randX, randY}] = 0; // start to clear
			gameMap[{tempRandX, tempRandY}] = 0; // goal to clear

			pZombie = new Zombie(
				vehicleTracker,
				this,
				position, //initial position
				RandFloat()*TwoPi, //start rotations
				Vector2D(0, 0),    //velocity
				1.0,			//mass
				10.0,			//max force
				2.0,            //max velocity
				1.0,			//max turn rate
				32);			//scale

			pZombie->SetMaxHealth(250);
			pZombie->SetHealth(250);
			pZombie->GetSteering()->SetPath(m_pPath->GetPath());
			pZombie->GetSteering()->GetPath()->LoopOff();
			pZombie->GetSteering()->PatrolOn();
			pZombie->GetSteering()->SeparationOn();
			pZombie->GetSteering()->WallAvoidanceOn();
			pZombie->GetFSM()->SetCurrentState(ZombiePatrol::Instance());
			Vehicles[vehicleTracker] = pZombie;
			vehicleTracker++;
		}
	}
	else if (currentLevel == 2)
	{
		//clear all the matrix with the ground tile
		for (int i = 1; i <= TILE_COUNT_HEIGHT; i++)
		{
			for (int j = 1; j <= TILE_COUNT_WIDTH; j++)
			{
				gameMap[{i, j}] = 0;
			}
		}

		randX = select_randomly(3, TILE_COUNT_WIDTH - 3);
		randY = select_randomly(3, TILE_COUNT_HEIGHT - 3);

		// Make 15 obstacles
		for (unsigned int i = 0; i < 15; i++)
		{
			while (gameMap[{randX, randY}] != 0)
			{
				randX = select_randomly(3, TILE_COUNT_WIDTH - 3);
				randY = select_randomly(3, TILE_COUNT_HEIGHT - 3);
			}

			pObstacle = new Obstacle(Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32), 32); // x = 3, y = 1, +64 to move
			m_Obstacles.push_back(pObstacle);
			gameMap[{randX, randY}] = 1;
		}

		//--------------Add Player---------------------
		randX = select_randomly(1, TILE_COUNT_WIDTH);
		randY = select_randomly(1, TILE_COUNT_HEIGHT);

		while (gameMap[{randX, randY}] != 0)
		{
			randX = select_randomly(1, TILE_COUNT_WIDTH);
			randY = select_randomly(1, TILE_COUNT_HEIGHT);
		}

		position = Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32);

		staticPlayer = new Player(
			vehicleTracker,
			this,
			position, //initial position
			RandFloat()*TwoPi, //start rotations
			Vector2D(0, 0),    //velocity
			1.0,			//mass
			1.0,			//max force
			4.0,            //max velocity
			1.0,			//max turn rate
			32);			//scale

		staticPlayer->GetSteering()->SetTarget(staticPlayer->GetPos());
		staticPlayer->GetSteering()->ArriveOn();
		staticPlayer->GetSteering()->ObstacleAvoidanceOn();
		Vehicles[vehicleTracker] = staticPlayer;
		vehicleTracker++;

		// [x][y]
		gameMap[{randX, randY}] = 1; // player position

		// Add 15 spiders
		for (int i = 0; i < 15; i++)
		{
			randX = select_randomly(1, TILE_COUNT_WIDTH);
			randY = select_randomly(1, TILE_COUNT_HEIGHT);

			while (gameMap[{randX, randY}] != 0)
			{
				randX = select_randomly(1, TILE_COUNT_WIDTH);
				randY = select_randomly(1, TILE_COUNT_HEIGHT);
			}

			position = Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32);

			pSpider = new Spider(
				vehicleTracker,
				this,
				position, //initial position
				RandFloat()*TwoPi, //start rotations
				Vector2D(0, 0),    //velocity
				1.0,			//mass
				10.0,			//max force
				3.0,            //max velocity
				1.0,			//max turn rate
				32);			//scale

			pSpider->SetMaxHealth(100);
			pSpider->SetHealth(100);
			pSpider->SetDamage(15);
			pSpider->GetSteering()->HideOn(staticPlayer);
			pSpider->GetSteering()->SeparationOn();
			pSpider->GetSteering()->WallAvoidanceOn();
			Vehicles[vehicleTracker] = pSpider;
			vehicleTracker++;
		}
	}
	else if (currentLevel == 3)
	{
		int counter = 1;
		Vector2D offsetPosition;

		//--------------Add Player---------------------
		randX = select_randomly(1, TILE_COUNT_WIDTH);
		randY = select_randomly(1, TILE_COUNT_HEIGHT);

		while (gameMap[{randX, randY}] != 0)
		{
			randX = select_randomly(1, TILE_COUNT_WIDTH);
			randY = select_randomly(1, TILE_COUNT_HEIGHT);
		}

		position = Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32);

		staticPlayer = new Player(
			vehicleTracker,
			this,
			position, //initial position
			RandFloat()*TwoPi, //start rotations
			Vector2D(0, 0),    //velocity
			1.0,			//mass
			1.0,			//max force
			4.0,            //max velocity
			1.0,			//max turn rate
			32);			//scale

		staticPlayer->GetSteering()->SetTarget(staticPlayer->GetPos());
		staticPlayer->GetSteering()->ArriveOn();
		Vehicles[vehicleTracker] = staticPlayer;
		vehicleTracker++;

		// [x][y]
		gameMap[{randX, randY}] = 1; // player position

		// Add 5 alien leaders
		for (int i = 0; i < 5; i++)
		{
			randX = select_randomly(1, TILE_COUNT_WIDTH);
			randY = select_randomly(1, TILE_COUNT_HEIGHT);

			while (gameMap[{randX, randY}] != 0)
			{
				randX = select_randomly(1, TILE_COUNT_WIDTH);
				randY = select_randomly(1, TILE_COUNT_HEIGHT);
			}

			position = Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32);

			pAlien = new Alien(
				vehicleTracker,
				this,
				position, //initial position
				RandFloat()*TwoPi, //start rotations
				Vector2D(0, 0),    //velocity
				1.0,			//mass
				20.0,			//max force
				2.0,            //max velocity
				1.0,			//max turn rate
				32);			//scale

			pAlien->SetMaxHealth(200);
			pAlien->SetHealth(200);
			pAlien->SetDamage(50);
			pAlien->SetColor(1.0f, 0.0f, 0.0f); // Red
			pAlien->SetAsLeader();
			pAlien->GetSteering()->WanderOn();
			pAlien->GetFSM()->SetCurrentState(AlienWander::Instance());
			pAlien->GetSteering()->WallAvoidanceOn();
			pAlien->GetSteering()->SeparationOn();
			Vehicles[vehicleTracker] = pAlien;
			vehicleTracker++;
		}

		// Add 4 alien followers for each alien leader
		for (int i = 1; i < 6; i++)
		{
			counter = 1;
			for (int j = 0; j < 4; j++)
			{
				if (counter == 1)
					offsetPosition = Vector2D(0, 96);
				else if (counter == 2)
					offsetPosition = Vector2D(0, -96);
				else if (counter == 3)
					offsetPosition = Vector2D(96, 0);
				else if (counter == 4)
					offsetPosition = Vector2D(-96, 0);

				position = Vehicles.at(i)->GetPos() + offsetPosition;

				if (position.x < 0)
					position.x *= -1;
				else if (position.x > MAP_WIDTH)
					position.x -= offsetPosition.x;
				if (position.y < 0)
					position.y *= -1;
				else if (position.y > MAP_HEIGHT)
					position.y -= offsetPosition.y;

				pAlien = new Alien(
					vehicleTracker,
					this,
					position, //initial position
					RandFloat()*TwoPi, //start rotations
					Vector2D(0, 0),    //velocity
					1.0,			//mass
					20.0,			//max force
					3.0,            //max velocity
					1.0,			//max turn rate
					32);			//scale

				pAlien->SetMaxHealth(150);
				pAlien->SetHealth(150);
				pAlien->SetDamage(10);
				pAlien->SetColor(1.0f, 1.0f, 1.0f);
				pAlien->GetSteering()->SetTargetAgent1(Vehicles.at(i));
				pAlien->GetSteering()->SetOffset(offsetPosition);
				pAlien->GetSteering()->OffsetPursuitOn(Vehicles.at(i), offsetPosition);
				pAlien->GetFSM()->SetCurrentState(AlienOffsetPursuit::Instance());
				pAlien->GetSteering()->WallAvoidanceOn();
				pAlien->GetSteering()->SeparationOn();
				Vehicles[vehicleTracker] = pAlien;
				vehicleTracker++;
				counter++;
			}
		}
	}
	else if (currentLevel == 4)
	{
		//--------------Add Player---------------------
		randX = 30;
		randY = 16;

		position = Vector2D((randX - 1) * 64 + 32, (randY - 1) * 64 + 32);

		staticPlayer = new Player(
			vehicleTracker,
			this,
			position, //initial position
			RandFloat()*TwoPi, //start rotations
			Vector2D(0, 0),    //velocity
			1.0,			//mass
			1.0,			//max force
			4.0,            //max velocity
			1.0,			//max turn rate
			32);			//scale

		staticPlayer->GetSteering()->SetTarget(staticPlayer->GetPos());
		staticPlayer->GetSteering()->ArriveOn();
		Vehicles[vehicleTracker] = staticPlayer;
		vehicleTracker++;

		// [x][y]
		gameMap[{randX, randY}] = 1; // player position

		Vector2D positions[5];
		positions[0] = Vector2D((5 - 1) * 64 + 32, (5 - 1) * 64 + 32);
		positions[1] = Vector2D((5 - 1) * 64 + 32, (25 - 1) * 64 + 32);
		positions[2] = Vector2D((16 - 1) * 64 + 32, (25 - 1) * 64 + 32);
		positions[3] = Vector2D((50 - 1) * 64 + 32, (5 - 1) * 64 + 32);
		positions[4] = Vector2D((50 - 1) * 64 + 32, (25 - 1) * 64 + 32);

		// Add 5 enemy shooters
		for (int i = 0; i < 5; i++)
		{
			position = positions[i];

			pEnemyShooter = new EnemyShooter(
				vehicleTracker,
				this,
				position, //initial position
				RandFloat()*TwoPi, //start rotations
				Vector2D(0, 0),    //velocity
				1.0,			//mass
				20.0,			//max force
				2.0,            //max velocity
				1.0,			//max turn rate
				32);			//scale

			pEnemyShooter->SetMaxHealth(200);
			pEnemyShooter->SetHealth(200);
			pEnemyShooter->SetColor(.5f, .5f, .5f);
			pEnemyShooter->GetSteering()->WanderOn();
			pEnemyShooter->GetFSM()->SetCurrentState(EnemyShooterWander::Instance());
			pEnemyShooter->GetSteering()->WallAvoidanceOn();
			pEnemyShooter->GetSteering()->SeparationOn();
			Vehicles[vehicleTracker] = pEnemyShooter;
			vehicleTracker++;
		}
	}
}
