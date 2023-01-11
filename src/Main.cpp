#include "SDL_net.h"

#include "MyGame.h"

using namespace std;

const char* IP_NAME = "localhost";
const Uint16 PORT = 55555;

bool is_running = true;

MyGame* game = new MyGame();

static int on_receive(void* socket_ptr) {
    TCPsocket socket = (TCPsocket)socket_ptr;

    const int message_length = 1024;

    char message[message_length];
    int received;

    // TODO: while(), rather than do
    do {
        received = SDLNet_TCP_Recv(socket, message, message_length);
        message[received] = '\0';

        char* pch = strtok(message, ",");

        // get the command, which is the first string in the message
        string cmd(pch);

        // then get the arguments to the command
        vector<string> args;

        while (pch != NULL) {
            pch = strtok(NULL, ",");

            if (pch != NULL) {
                args.push_back(string(pch));
            }
        }

        game->on_receive(cmd, args);

        if (cmd == "exit") {
            break;
        }

        // if told to disconnect, end game loop
        if (cmd == "disconnect")
        {
            is_running = false;
        }

    } while (received > 0 && is_running);

    return 0;
}

static int on_send(void* socket_ptr) {
    TCPsocket socket = (TCPsocket)socket_ptr;

    while (is_running) {
        if (game->messages.size() > 0) {
            string message = "CLIENT_DATA";

            for (auto m : game->messages) {
                message += "," + m;
            }

            game->messages.clear();

            cout << "Sending_TCP: " << message << endl;

            SDLNet_TCP_Send(socket, message.c_str(), message.length());
        }

        SDL_Delay(1);
    }

    return 0;
}

void loop(SDL_Renderer* renderer) {
    SDL_Event event;

    while (is_running) {
        // input
        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0) {
                game->input(event);

                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    game->send("DISCONNECTED");
                    break;

                default:
                    break;
                }
            }

            if (event.type == SDL_QUIT) {
                game->send("DISCONNECTED");
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        game->update();

        game->render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(17); // 17ms = 1/60 seconds
    }
}

void menuLoop(SDL_Renderer* renderer)
{

    bool inMenu = true;

    TTF_Font* pongFont = TTF_OpenFont("pong.ttf", 40);

    int texW, texH = 0;

    SDL_Color menuColor = { 255,255,255 };
    SDL_Surface* surface = TTF_RenderText_Solid(pongFont, "Start Game", menuColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int texXPos = 50;
    int texYPos = 100;

    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { texXPos,texYPos, texW, texH };

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);

    SDL_Event event;

    while (inMenu)
    {
        while (SDL_PollEvent(&event))
        {
            int x, y;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                std::cout << "Click" << endl;
                if (SDL_GetMouseState(&x, &y))
                {
                    if (x < texXPos)
                    {

                    }
                    else if (x > texXPos + texW)
                    {

                    }
                    else if (y < texYPos)
                    {

                    }
                    else if (y > texYPos + texH)
                    {

                    }
                    else
                    {
                        std::cout << "Button pressed" << endl;
                        inMenu = false;
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    //end program
                }
            }
        }
    }

    SDL_RenderClear(renderer);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    TTF_CloseFont(pongFont);

}

int run_game() {
    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Pong Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (nullptr == window) {
        std::cout << "Failed to create window" << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    game->loadResources(renderer);

    menuLoop(renderer);

    if (nullptr == renderer) {
        std::cout << "Failed to create renderer" << SDL_GetError() << std::endl;
        return -1;
    }


    loop(renderer);


    return 0;
}

int main(int argc, char** argv) {

    // Initialize SDL
    if (SDL_Init(0) == -1) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    // Initialize SDL_net
    if (SDLNet_Init() == -1) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);
    }

    IPaddress ip;

    // Resolve host (ip name + port) into an IPaddress type
    if (SDLNet_ResolveHost(&ip, IP_NAME, PORT) == -1) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(3);
    }

    // Initialize font
    TTF_Init();

    // Initialize SDL_Mixer
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // Open the connection to the server
    TCPsocket socket = SDLNet_TCP_Open(&ip);

    if (!socket) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(4);
    }

    SDL_CreateThread(on_receive, "ConnectionReceiveThread", (void*)socket);
    SDL_CreateThread(on_send, "ConnectionSendThread", (void*)socket);

    // Send to the server that the client has connected
    game->send("CONNECTED");

    srand(time(NULL));

    run_game();

    delete game;

    // Shutdown font;
    TTF_Quit();

    // Close connection to the server
    SDLNet_TCP_Close(socket);

    // Shutdown SDL_net
    SDLNet_Quit();

    // Shutdown SDL
    SDL_Quit();

    return 0;
}