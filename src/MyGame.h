#ifndef __MY_GAME_H__
#define __MY_GAME_H__

#include <iostream>
#include <vector>
#include <string>

#include "SDL.h"
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "Particle.h"

static struct GameData {
    int player1Y = 0;
    int player2Y = 0;
    int ballX = 0;
    int ballY = 0;
    int player1Score = 0;
    int player2Score = 0;

} game_data;

class MyGame {

private:
    // Create objects
    //SDL_Rect player1 = { 200, 0, 20, 60 };
    //SDL_Rect player2 = { 580, 0, 20, 60 };

    SDL_Texture* player1;
    SDL_Texture* player2;

    SDL_Texture* ball;

    SDL_Texture* background;

    std::vector<Particle*> particles;

    Mix_Chunk* scoreSound;


public:
    std::vector<std::string> messages;

    void on_receive(std::string message, std::vector<std::string>& args);
    void send(std::string message);
    void input(SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer);
    void loadResources(SDL_Renderer* renderer);
    void closeResources();

};

#endif