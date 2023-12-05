#include <SDL2/SDL.h>
#include <cmath>
const int thickness = 15;
const int paddleHeight = 100;

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
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	Vector2 mPaddlePos;
	Vector2 mBallPos;
	Vector2 mBallVelocity;
	int mPaddleDir;

	bool mIsRunning;
	Uint32 mTicksCount;
};

Game::Game()
{
	mWindow = nullptr;
	mRenderer = nullptr;
	mIsRunning = true;
	mTicksCount = 0;
	mPaddleDir = 0;
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
	mBallVelocity.x = -215.0f;
	mBallVelocity.y = 235.0f;

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

	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}

}

void Game::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;
	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

		// Make sure the paddle doesnt move off screen
		if (mPaddlePos.y < (paddleHeight/2.0f + thickness))
		{
			mPaddlePos.y = paddleHeight/2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleHeight/2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleHeight/2.0f - thickness;
		}
	}

	mBallPos.x += mBallVelocity.x * deltaTime;
	mBallPos.y += mBallVelocity.y * deltaTime;

	if (mBallPos.y <= thickness && mBallVelocity.y < 0.0f)
	{
		mBallVelocity.y *= -1;
	}

	if (mBallPos.x >= 1024 - thickness && mBallVelocity.x > 0.0f)
	{
		mBallVelocity.x *= -1;
	}

	if (mBallPos.y >= 768 - thickness && mBallVelocity.y > 0.0f)
	{
		mBallVelocity.y *= -1;
	}

	float diff = std::abs(mBallPos.y - mPaddlePos.y);
	if (
		// Our y-difference is small enough
		diff <= paddleHeight / 2.0f &&
		// We are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// The ball is moving to the left
		mBallVelocity.x < 0.0f
		)
	{
		mBallVelocity.x *= -1.0f;
	}

}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw wall to the top of the screen
	SDL_Rect wall{0,0,1024,thickness};
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw wall to the bottome of the screen
	wall.y = 768-thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw wall to the right of the screen
	wall.y = 0;
	wall.x = 1024 - thickness;
	wall.w = thickness;
	wall.h = 768;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw a ball in the middle of the screen
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);

	// Draw a paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleHeight/2),
		thickness,
		paddleHeight
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


