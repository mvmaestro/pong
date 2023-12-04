#include <SDL2/SDL.h>
const int GLOBAL_CONST_THICKNESS = 15;
const int GLOBAL_CONST_PADDLE_SIZE = 100;

struct Vector2
{
	float x;
	float y;
};

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame() {};
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	Vector2 mPaddlePos;
	Vector2 mBallPos;

	bool mIsRunning;
};

Game::Game()
{
	mWindow = nullptr;
	mRenderer = nullptr;
	mIsRunning = true;
}

bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"Game Programming Chapter 1",
		100,
		100,
		1024,
		768,
		0
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f/2.0f;

	return true;
}

void Game::Shutdown()
{
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) // SDL event is taken by pointer so any information is stored about the event removed from the queue
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw wall to the top of the screen
	SDL_Rect wall{0,0,1024,GLOBAL_CONST_THICKNESS};
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw wall to the bottome of the screen
	wall.y = 768-GLOBAL_CONST_THICKNESS;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw wall to the right of the screen
	wall.y = 0;
	wall.x = 1024 - GLOBAL_CONST_THICKNESS;
	wall.w = GLOBAL_CONST_THICKNESS;
	wall.h = 768;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw a ball in the middle of the screen
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - GLOBAL_CONST_THICKNESS/2),
		static_cast<int>(mBallPos.y - GLOBAL_CONST_THICKNESS/2),
		GLOBAL_CONST_THICKNESS,
		GLOBAL_CONST_THICKNESS
	};
	SDL_RenderFillRect(mRenderer, &ball);

	// Draw a paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - GLOBAL_CONST_PADDLE_SIZE/2),
		GLOBAL_CONST_THICKNESS,
		GLOBAL_CONST_PADDLE_SIZE
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	SDL_RenderPresent(mRenderer);
}



int main(int argc, char** argv)
{
	Game game;
	bool success = game.Initialize();
	if (success)
	{
		game.RunLoop();
	}
	game.Shutdown();
	return 0;
}


