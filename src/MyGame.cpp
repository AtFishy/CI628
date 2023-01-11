#include "MyGame.h"

TTF_Font* scoreFont;

// Receiving data from server
void MyGame::on_receive(std::string cmd, std::vector<std::string>& args) {
    if (cmd == "GAME_DATA") {
        // we should have exactly 4 arguments
        if (args.size() == 4) {
            game_data.player1Y = stoi(args.at(0));
            game_data.player2Y = stoi(args.at(1));
            game_data.ballX = stoi(args.at(2));
            game_data.ballY = stoi(args.at(3));
        }
    }
    else if (cmd == "SCORES")
    {
        Mix_PlayChannel(-1, scoreSound, 0);

        std::cout << "Received: " << cmd << " || " << std::endl;
        if (game_data.ballX < 100)
        {

            game_data.player2Score = stoi(args.at(0));
            // Generate text
        }
        else
        {
            game_data.player1Score = stoi(args.at(0));

        }
        std::cout << game_data.player1Score << " || " << game_data.player2Score << std::endl;
    }
    else
    {
        std::cout << cmd << std::endl;
    }
}

void MyGame::send(std::string message) {
    messages.push_back(message);
}

// Handles input
void MyGame::input(SDL_Event& event) {
    switch (event.key.keysym.sym) {
    case SDLK_w:
        send(event.type == SDL_KEYDOWN ? "W_DOWN" : "W_UP");
        break;
    case SDLK_s:
        send(event.type == SDL_KEYDOWN ? "S_DOWN" : "S_UP");
        break;
    }
}

void MyGame::update() {
    //player1.y = game_data.player1Y;
   // player2.y = game_data.player2Y;   

    for (int p = 0; p < particles.size(); p++)
    {
        particles[p]->updateParticle();
        if (particles[p]->checkIsDead())
        {
            delete particles[p];
            particles[p] = new Particle(game_data.ballX + 10, game_data.ballY + 10);
        }
    }
}

// Create text, texture, size and display it
void DisplayFont(SDL_Renderer* renderer)
{
    // Construct score text and covert it to correct type
    std::string scoreText = "Scores: " + std::to_string(game_data.player1Score) + " " + std::to_string(game_data.player2Score);

    char* cstr = new char[scoreText.length() + 1];
    std::strcpy(cstr, scoreText.c_str());

    int texW = 0;
    int texH = 0;

    // Create texture to display
    SDL_Color scoreColor = { 255,255,255 };
    SDL_Surface* surface = TTF_RenderText_Solid(scoreFont, cstr, scoreColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { 300,0, texW, texH };

    // Render text
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);


    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

}

void MyGame::render(SDL_Renderer* renderer) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Rect backgroundDstrect = { 0,0,800,600 };
    SDL_RenderCopy(renderer, background, NULL, &backgroundDstrect);

    SDL_Rect player1dstrect = { 200, game_data.player1Y, 20, 60 };
    SDL_RenderCopy(renderer, player1, NULL, &player1dstrect);

    SDL_Rect player2dstrect = { 580, game_data.player2Y, 20, 60 };
    SDL_RenderCopy(renderer, player2, NULL, &player2dstrect);

    SDL_Rect ballDstrect = { game_data.ballX, game_data.ballY, 20, 20 };
    SDL_RenderCopy(renderer, ball, NULL, &ballDstrect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);

    for each (Particle * p in particles)
    {

        p->renderParticle(renderer);

    }

    // Display scores
    DisplayFont(renderer);

}

void OnBatHit()
{
    // Play audio
    // Wiggle bat
    // Emit particles
    // Angle variable that decays towards 0 and switches between positive and negative.
}

void MyGame::loadResources(SDL_Renderer* renderer)
{
    // Load font
    scoreFont = TTF_OpenFont("pong.ttf", 25);

    // Load images
    SDL_Surface* ballSurface = IMG_Load("Ball.png");
    ball = SDL_CreateTextureFromSurface(renderer, ballSurface);
    if (ball != NULL)
    {
        std::cout << "Image loaded\n";
    }
    SDL_FreeSurface(ballSurface);

    SDL_Surface* p1Surface = IMG_Load("player1.png");
    player1 = SDL_CreateTextureFromSurface(renderer, p1Surface);
    if (player1 != NULL)
    {
        std::cout << "Image loaded\n";
    }
    SDL_FreeSurface(p1Surface);

    SDL_Surface* p2Surface = IMG_Load("player2.png");
    player2 = SDL_CreateTextureFromSurface(renderer, p2Surface);
    if (player2 != NULL)
    {
        std::cout << "Image loaded\n";
    }
    SDL_FreeSurface(p2Surface);

    SDL_Surface* backgroundSurface = IMG_Load("background.png");
    background = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    if (background != NULL)
    {
        std::cout << "Image loaded\n";
    }
    SDL_FreeSurface(backgroundSurface);

    // Display background
    SDL_Rect backgroundRect = { 0,0,800,600 };
    SDL_RenderCopy(renderer, background, NULL, &backgroundRect);


    // Load sound
    scoreSound = Mix_LoadWAV("scoreSound.wav");


    // Load particles
    for (int i = 0; i < 20; i++)
    {
        particles.push_back(new Particle(game_data.ballX, game_data.ballY));
    }

}

void MyGame::closeResources()
{
    TTF_CloseFont(scoreFont);
}