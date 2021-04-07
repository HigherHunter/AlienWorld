#ifndef GameWorld_H
#define GameWorld_H

#include "Vehicle.h"
#include "Globals.h"

class Bullet;
class Vehicle;
class Player;
class EnemyShooter;

class GameWorld
{
private:

	bool gameOver = false, gameWon = false, gamePaused = false;

	unsigned int currentLevel = 1, killCount = 0, noOfEnemies = 0;

	// Score made by killing enemies
	int killScore;

	// World knows about player
	Player* staticPlayer;

	// vehicle track number
	unsigned int vehicleTracker = 0;
	// vehicles to be destroyed
	std::vector<Vehicle*> m_destroyv;
	std::vector<Vehicle*>::iterator vDestoryIt;

	// all neighbours in radius
	std::vector<Vehicle*> m_Neighbours;

	// A container of any obstacles
	std::vector<BaseGameEntity*> m_Obstacles;

	// bullets tracker
	unsigned int bulletTracker = 0;
	// bullets to be destroyed
	std::vector<Bullet*> m_destroyb;
	std::vector<Bullet*>::iterator bDestroyIt;

	// Any path we may create for the vehicles to follow
	Path* m_pPath;

	// Local copy of client window dimensions
	int m_WindowSizeX, m_WindowSizeY;

	// Current game time
	std::chrono::high_resolution_clock::time_point gameTime = std::chrono::high_resolution_clock::now();

	// Current time
	std::chrono::high_resolution_clock::time_point currentTime;

	std::chrono::high_resolution_clock::time_point killTime;

	// Gives constant tick for fixed update
	bool FixedUpdateTime();

	// Makes A* path for vehicles
	void PathInitialization();

	// Designes map according to level
	void LevelDesignInitialization();

public:

	GameWorld(int cx, int cy, unsigned int level);

	~GameWorld();

	void Update();

	const std::vector<BaseGameEntity*>& GetObstacles()const { return m_Obstacles; }

	const std::vector<Vehicle*>& GetNeighbours() { return m_Neighbours; }

	Vehicle* GetAgentAt(unsigned int key);
	Player* GetPlayer()const { return staticPlayer; }

	int GetWindowSizeX()const { return m_WindowSizeX; }
	int GetWindowSizeY()const { return m_WindowSizeY; }

	void CreatePlayerBullet();
	void CreateEnemyShooterBullet(EnemyShooter* shooter);
	void RemoveBullet(Bullet* b);

	void RemoveAgent(Vehicle* v);

	bool ShouldShowIndicator();

	void TagVehiclesWithinViewrange(Vehicle* pVehicle, double range);

	Vehicle* FindClosestEnemy();

	bool IsPlayerWithinRadius(Vehicle* pVehicle, double range);

	bool IsGameOver()const { return gameOver; }
	bool IsGameWon()const { return gameWon; }

	bool IsGamePaused()const { return gamePaused; }
	void GamePausedOn();
	void GamePausedOff();

	unsigned int GetEnemiesLeft()const { return noOfEnemies - killCount; }

	int GetKillScore()const { return killScore; }

	unsigned int GetCurrentLevel()const { return currentLevel; }
	void SetCurrentLevel(unsigned int level) { currentLevel = level; }
};

#endif