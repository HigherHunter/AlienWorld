#include "GameWorld.h"
#include "OpenGl.h"
#include "DialogBoxDisplay.h"
#include "Player.h"

bool gameActive = false;
bool paused = false;
bool justStarted = true;
bool displayHighScore = false;
bool displayAboutGame = false;

int FPS = 1000/ 60; // 60 frames per second

std::chrono::system_clock::time_point startTime;

int timeLeft = 300; // 300 seconds - 5 minutes

int score = 0;
int levelScore = 0;
int highScore = 0;

// Mouse x and y positon on screen
GLdouble mousePosX, mousePosY;

// pointers
GameWorld *world;
DialogBoxDisplay *startMenu;
DialogBoxDisplay *pauseMenu;
DialogBoxDisplay *gameOverScreen;
DialogBoxDisplay *gameWonScreen;

// Pointer to player
Player* playerEntity;

// Keys on keyboard
bool keystates[256];

// Background image
GLuint background;

// Culling size
GLdouble left, right, bottom, top;

std::string s; // temp holder
char const *scoreDisplay;
char const *highScoreDisplay;
char const *enemiesLeftDisplay;
char const *timeLeftDisplay;

// temp player position holder
Vector2D playerPosHolder;

void ChangeTime()
{
	std::chrono::system_clock::time_point tempTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = tempTime - startTime;
	if ((int)(elapsed_seconds.count()) >= 1)
	{
		startTime = std::chrono::system_clock::now();
		timeLeft--;
		if (timeLeft < 0)
			timeLeft = 0;
	}
}

void WriteHighScore()
{
	std::ofstream outfile;
	outfile.open("highScore.txt");
	if (score > highScore)
		highScore = score;
	outfile << highScore << std::endl;
	outfile.close();
}

void ReadHighScore()
{
	std::ifstream infile;
	infile.open("highScore.txt");
	infile >> highScore;
	infile.close();
}

void Display(void)
{
	//clear all pixels with the specified clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f, 1.0f, 1.0f); // White reset

	glBindTexture(GL_TEXTURE_2D, background);
	// Apply texture
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(MAP_WIDTH, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(MAP_WIDTH, MAP_HEIGHT);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0, MAP_HEIGHT);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	if (justStarted)
	{
		startMenu->RenderDialogBox(left + WINWIDTH / 2, bottom + WINHEIGHT / 2, mousePosX, mousePosY);

		// Text display color
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow text

		if (displayHighScore)
		{
			s = std::to_string(highScore);
			highScoreDisplay = s.c_str();
			drawStrokeText((char*)"High score:", WINWIDTH / 2 - 50, WINHEIGHT - 50);
			drawStrokeText((char*)highScoreDisplay, WINWIDTH / 2 - 25, WINHEIGHT - 75);
		}
		else if (displayAboutGame)
		{
			drawStrokeText((char*)"Alien World is a dangerous place.", WINWIDTH / 2 - 100, WINHEIGHT - 50);
			drawStrokeText((char*)"Keep your gun close.", WINWIDTH / 2 - 100, WINHEIGHT - 75);
			drawStrokeText((char*)"But keep your enemies far away.", WINWIDTH / 2 - 100, WINHEIGHT - 100);
		}
	}
	else if (paused)
		pauseMenu->RenderDialogBox(left + WINWIDTH / 2, bottom + WINHEIGHT / 2, mousePosX, mousePosY);
	else if (world->IsGameOver())
	{
		gameActive = false;
		gameOverScreen->RenderDialogBox(left + WINWIDTH / 2, bottom + WINHEIGHT / 2, mousePosX, mousePosY);
	}
	else if (world->IsGameWon())
	{
		gameActive = false;
		gameWonScreen->RenderDialogBox(left + WINWIDTH / 2, bottom + WINHEIGHT / 2, mousePosX, mousePosY);
	}
	else if (gameActive)
	{
		if (timeLeft != 0)
			ChangeTime();
		// screen positions for camera where player is in the center
		left = playerEntity->GetPos().x - WINWIDTH / 2;
		right = left + WINWIDTH;
		bottom = playerEntity->GetPos().y - WINHEIGHT / 2;
		top = bottom + WINHEIGHT;
		if (left < 0)
		{
			right += -left;
			left = 0;
		}
		if (right > MAP_WIDTH)
		{
			left += -(right - MAP_WIDTH);
			right = MAP_WIDTH;
		}
		if (bottom < 0)
		{
			top += -bottom;
			bottom = 0;
		}
		if (top > MAP_HEIGHT)
		{
			bottom += -(top - MAP_HEIGHT);
			top = MAP_HEIGHT;
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Set where player is looking
		if (playerEntity->IsAlive())
		{
			playerEntity->SetLookingVec
			(Vector2D(mousePosX + playerEntity->GetPos().x - playerPosHolder.x,
				mousePosY + playerEntity->GetPos().y - playerPosHolder.y));
		}

		// Update whole world
		world->Update();

		// Change to projection and update camera
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluOrtho2D(left, right, bottom, top);

		// Players bacground health
		glColor3f(1.0f, 0.0f, 0.0f); // Red
		glBegin(GL_QUADS);
		glVertex2f(left + 25, top - 50);
		glVertex2f(left + 25 + 200, top - 50);
		glVertex2f(left + 25 + 200, top - 25);
		glVertex2f(left + 25, top - 25);
		glEnd();

		// Players health
		glColor3f(0.0f, 1.0f, 0.0f); // Green
		glBegin(GL_QUADS);
		glVertex2f(left + 25, top - 50);
		glVertex2f(left + 25 + 200 - (200 * playerEntity->HealthPercentLost() / 100), top - 50);
		glVertex2f(left + 25 + 200 - (200 * playerEntity->HealthPercentLost() / 100), top - 25);
		glVertex2f(left + 25, top - 25);
		glEnd();

		// Text display color
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow text
		// Draw text
		s = std::to_string(world->GetEnemiesLeft());
		enemiesLeftDisplay = s.c_str();
		drawStrokeText((char*)"Enemies left:", right - 200, top - 50);
		drawStrokeText((char*)enemiesLeftDisplay, right - 50, top - 50);

		levelScore = world->GetKillScore();

		s = std::to_string(score + levelScore);
		scoreDisplay = s.c_str();
		drawStrokeText((char*)"Score:", left + WINWIDTH / 2 - 50, top - 50);
		drawStrokeText((char*)scoreDisplay, left + WINWIDTH / 2 + 50, top - 50);

		s = std::to_string(timeLeft);
		timeLeftDisplay = s.c_str();
		drawStrokeText((char*)"Time left:", left + WINWIDTH / 2 - 50, top - 100);
		drawStrokeText((char*)timeLeftDisplay, left + WINWIDTH / 2 + 50, top - 100);
	}

	// swap the buffers
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	// on reshape and on startup, keep the viewport to be the entire size of the window
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// keep our logical coordinate system constant
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
}

void LoadBackground()
{
	if (world->GetCurrentLevel() == 1)
		background = LoadTexture((char*)"Assets\\Sprites\\Background\\background1.png");
	else if (world->GetCurrentLevel() == 2)
		background = LoadTexture((char*)"Assets\\Sprites\\Background\\background2.png");
	else if (world->GetCurrentLevel() == 3)
		background = LoadTexture((char*)"Assets\\Sprites\\Background\\background3.png");
	else if (world->GetCurrentLevel() == 4)
		background = LoadTexture((char*)"Assets\\Sprites\\Background\\background2.png");
}

void GameRestart(int level)
{
	levelScore = 0;
	delete world;
	delete playerEntity;
	world = new GameWorld(WINWIDTH, WINHEIGHT, level);
	playerEntity = world->GetPlayer();
	LoadBackground();
	timeLeft = 300;
	paused = false;
	gameActive = true;
	startTime = std::chrono::system_clock::now();
}

void init(void)
{
	//set the clear color to be black
	glClearColor(1.0, 1.0, 1.0, 1.0);

	world = new GameWorld(WINWIDTH, WINHEIGHT, 1);

	playerEntity = world->GetPlayer();

	LoadBackground();

	startMenu = new DialogBoxDisplay(250, 250, std::vector<std::string>{"New Game", "High Score", "About Game"});
	pauseMenu = new DialogBoxDisplay(250, 250, std::vector<std::string>{"Resume", "Restart", "Quit"});
	gameOverScreen = new DialogBoxDisplay(250, 250, std::vector<std::string>{"Restart", "Quit"});
	gameWonScreen = new DialogBoxDisplay(250, 250, std::vector<std::string>{"Next", "Quit"});

	ReadHighScore();
}

void MouseOrEnterCheck()
{
	if (justStarted)
	{
		if (startMenu->GetCurrentSelection() == 1)
		{
			justStarted = false;
			gameActive = true;
			startTime = std::chrono::system_clock::now();
		}
		else if (startMenu->GetCurrentSelection() == 2)
		{
			displayAboutGame = false;
			displayHighScore = true;
		}
		else if (startMenu->GetCurrentSelection() == 3)
		{
			displayHighScore = false;
			displayAboutGame = true;
		}
	}
	else if (paused)
	{
		if (pauseMenu->GetCurrentSelection() == 1)
		{
			paused = false;
			world->GamePausedOff();
			gameActive = true;
		}
		else if (pauseMenu->GetCurrentSelection() == 2)
			GameRestart(world->GetCurrentLevel());
		else if (pauseMenu->GetCurrentSelection() == 3)
		{
			if (timeLeft != 0)
				score += levelScore + timeLeft / world->GetEnemiesLeft();
			else
				score += levelScore;
			WriteHighScore();
			exit(0);
		}
	}
	else if (world->IsGameOver())
	{
		if (gameOverScreen->GetCurrentSelection() == 1)
		{
			GameRestart(world->GetCurrentLevel());
		}
		else if (gameOverScreen->GetCurrentSelection() == 2)
		{
			score += levelScore + timeLeft;
			WriteHighScore();
			exit(0);
		}
	}
	else if (world->IsGameWon())
	{
		if (timeLeft != 0)
			score += levelScore + playerEntity->GetHealth() * timeLeft;
		else
			score += levelScore + playerEntity->GetHealth();
		if (gameWonScreen->GetCurrentSelection() == 1)
		{
			unsigned int nextLevel = world->GetCurrentLevel() + 1;
			if (nextLevel > LEVELS_COUNT) {}
			else
			{
				GameRestart(nextLevel);
			}
		}
		else if (gameWonScreen->GetCurrentSelection() == 2)
		{
			WriteHighScore();
			exit(0);
		}
	}
}

void onMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (gameActive)
		{
			if (playerEntity->IsAlive())
				world->CreatePlayerBullet();
		}
		else
			MouseOrEnterCheck();
	}
}

void onMouseMotion(int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	mousePosX = posX;
	mousePosY = posY;

	playerPosHolder = playerEntity->GetPos();
}

void KeyboardSpecial(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		if (justStarted)
			startMenu->SetPrevMenuItem();
		else if (paused)
			pauseMenu->SetPrevMenuItem();
		else if (world->IsGameOver())
			gameOverScreen->SetPrevMenuItem();
		else if (world->IsGameWon())
			gameWonScreen->SetPrevMenuItem();
		break;
	case GLUT_KEY_DOWN:
		if (justStarted)
			startMenu->SetNextMenuItem();
		else if (paused)
			pauseMenu->SetNextMenuItem();
		else if (world->IsGameOver())
			gameOverScreen->SetNextMenuItem();
		else if (world->IsGameWon())
			gameWonScreen->SetNextMenuItem();
		break;
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
	}
}

void KeyboardDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // Escape
		if (gameActive)
		{
			paused = true;
			world->GamePausedOn();
			gameActive = false;
		}
		else if (paused)
		{
			paused = false;
			world->GamePausedOff();
			gameActive = true;
		}
		break;
	case 13: // Enter
		MouseOrEnterCheck();
		break;
	}
	keystates[key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	keystates[key] = false;
}

void GetPressedKeys()
{
	Vector2D target;

	if (keystates[119] && keystates[97]) // w and a 
	{
		target = Vector2D(playerEntity->GetPos().x - playerEntity->GetBRadius(), playerEntity->GetPos().y + playerEntity->GetBRadius());
		if (target.x > 0 && target.y < MAP_HEIGHT)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else if (target.x < 0)
		{
			target = Vector2D(0 + playerEntity->GetBRadius(), playerEntity->GetPos().y);
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(playerEntity->GetPos().x, MAP_HEIGHT - playerEntity->GetBRadius());
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
	else if (keystates[119] && keystates[100]) // w and d
	{
		target = Vector2D(playerEntity->GetPos().x + playerEntity->GetBRadius(), playerEntity->GetPos().y + playerEntity->GetBRadius());
		if (target.x < MAP_WIDTH && target.y < MAP_HEIGHT)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else if (target.x > MAP_WIDTH)
		{
			target = Vector2D(MAP_WIDTH - playerEntity->GetBRadius(), playerEntity->GetPos().y);
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(playerEntity->GetPos().x, MAP_HEIGHT - playerEntity->GetBRadius());
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
	else if (keystates[115] && keystates[97]) // s and a
	{
		target = Vector2D(playerEntity->GetPos().x - playerEntity->GetBRadius(), playerEntity->GetPos().y - playerEntity->GetBRadius());
		if (target.x > 0 && target.y > 0)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else if (target.x < 0)
		{
			target = Vector2D(0 + playerEntity->GetBRadius(), playerEntity->GetPos().y);
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(playerEntity->GetPos().x, 0 + playerEntity->GetBRadius());
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
	else if (keystates[115] && keystates[100]) // s and d
	{
		target = Vector2D(playerEntity->GetPos().x + playerEntity->GetBRadius(), playerEntity->GetPos().y - playerEntity->GetBRadius());
		if (target.x < MAP_WIDTH && target.y > 0)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else if (target.x > MAP_WIDTH)
		{
			target = Vector2D(MAP_WIDTH - playerEntity->GetBRadius(), playerEntity->GetPos().y);
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(playerEntity->GetPos().x, 0 + playerEntity->GetBRadius());
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
	else if (keystates[119]) // w
	{
		target = Vector2D(playerEntity->GetPos().x, playerEntity->GetPos().y + playerEntity->GetBRadius() * 2);
		if (target.y < MAP_HEIGHT)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(playerEntity->GetPos().x, MAP_HEIGHT - playerEntity->GetBRadius());
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
	else if (keystates[115]) // s
	{
		target = Vector2D(playerEntity->GetPos().x, playerEntity->GetPos().y - playerEntity->GetBRadius() * 2);
		if (target.y > 0)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(playerEntity->GetPos().x, 0 + playerEntity->GetBRadius());
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
	else if (keystates[97]) // a
	{
		target = Vector2D(playerEntity->GetPos().x - playerEntity->GetBRadius() * 2, playerEntity->GetPos().y);
		if (target.x > 0)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(0 + playerEntity->GetBRadius(), playerEntity->GetPos().y);
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
	else if (keystates[100]) // d
	{
		target = Vector2D(playerEntity->GetPos().x + playerEntity->GetBRadius() * 2, playerEntity->GetPos().y);
		if (target.x < MAP_WIDTH)
		{
			playerEntity->GetSteering()->SetTarget(target);
		}
		else
		{
			target = Vector2D(MAP_WIDTH - playerEntity->GetBRadius(), playerEntity->GetPos().y);
			playerEntity->GetSteering()->SetTarget(target);
		}
	}
}

void Timer(int value) {
	glutPostRedisplay();    // Post a paint request to activate display()
	glutTimerFunc(FPS, Timer, 0); // subsequent timer call at milliseconds
	GetPressedKeys();
}

int main(int argc, char* argv[])
{
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINWIDTH, WINHEIGHT);
	glutInitWindowPosition(300, 100);
	glutCreateWindow("Alien World");

	init();

	glutTimerFunc(0, Timer, 0);
	glutDisplayFunc(Display);
	glutReshapeFunc(reshape);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Mouse
	glutMouseFunc(onMouse);
	glutPassiveMotionFunc(onMouseMotion);

	// Keyboard
	glutDeviceGet(GLUT_DEVICE_IGNORE_KEY_REPEAT);
	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(KeyboardSpecial);

	glutMainLoop();
}