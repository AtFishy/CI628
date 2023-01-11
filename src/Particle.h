#pragma once
#include "SDL.h"
#include <stdlib.h>

class Particle
{

private:

	int lifetime = 0;
	int maxLifetime;

	int maxAmount = 1000;

	int oldX = 0;
	int oldY = 0;
	int newX = 0;
	int newY = 0;

	int xVel;
	int yVel;

	bool isDead;

	SDL_Rect rect = {0,0,5,5};

public:

	Particle(int x, int y)
	{
		oldX = x;
		oldY = y;
		
		// Randomise this depending on ball direction. Always make them go behind.
		xVel = (rand() % 5) + 1;
		yVel = (rand() % 3) + 1;

		// Randomise lifetime so not all spawn and die at the same time
		maxLifetime = (rand() % 60) + 60;

	}

	bool checkIsDead()
	{
		return maxLifetime == lifetime;
	}

	void renderParticle(SDL_Renderer* renderer)
	{

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

		SDL_RenderFillRect(renderer, &rect);

	}

	void updateParticle()
	{

		lifetime += 1;

		if (!checkIsDead())
		{
			newX = oldX + xVel;
			oldX = newX;
			newY = oldY + yVel;
			oldY = newY;

			rect = { newX, newY, 5, 5 };
		}

	}
};