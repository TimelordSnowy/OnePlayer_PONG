#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

const int FPS = 30;
const int FRAME_DELAY = 1000 / FPS;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window *window = nullptr;
SDL_Surface *sprite = nullptr;
SDL_Surface *paddleSprite = nullptr;
SDL_Surface *backGroundImage = nullptr;
SDL_Surface *backBuffer = nullptr;

Mix_Chunk *hitSound = nullptr;
Mix_Music *backGroundMusic = nullptr;

TTF_Font *gameFont = nullptr;

float inputDirectionX = 0.0f;
float inputDirectionY = 0.0f;
float movementSpeed = 5.0f;
float score = 0.0f;
float highscore = 0.0f;
int volume = 32;

bool isSpacePressed = false;
bool isAPressed = false;

float ballXVel = 0.0f;
float ballYVel = 0.0f;
float ballMovementSpeed = 5.0f;

SDL_Rect ballRect;
SDL_Rect paddleRect;
SDL_Rect leftRect;
SDL_Rect rightRect;
SDL_Rect topRect;
SDL_Rect bottomRect;

bool LoadFiles();
void FreeFiles();
bool ProgramIsRunning();
SDL_Surface *LoadImage(const char *fileName);
void DrawImage(SDL_Surface *image, SDL_Surface *destSurface, int x, int y);
void DrawImageFrame(SDL_Surface *image, SDL_Surface *destSurface, int x, int y, int width, int height, int frame);
void DrawText(SDL_Surface *surface, const char *string, int x, int y, TTF_Font *font, Uint8 r, Uint8 g, Uint8 b);
bool RectsOverlap(SDL_Rect rect1, SDL_Rect rect2);

int main(int argc, char *args[])
{
    std::cout << "Hello World" << std::endl;
    std::srand(std::time(NULL));

    paddleRect.x = SCREEN_WIDTH / 2 - 95 / 2;
    paddleRect.y = 500;
    paddleRect.w = 95;
    paddleRect.h = 55;

    leftRect.x = -5;
    leftRect.y = 0;
    leftRect.w = 5;
    leftRect.h = SCREEN_HEIGHT;

    rightRect.x = SCREEN_WIDTH;
    rightRect.y = 0;
    rightRect.w = 5;
    rightRect.h = SCREEN_HEIGHT;

    topRect.x = 0;
    topRect.y = 0;
    topRect.w = SCREEN_WIDTH;
    topRect.h = 5;

    bottomRect.x = -5;
    bottomRect.y = SCREEN_HEIGHT;
    bottomRect.w = SCREEN_WIDTH;
    bottomRect.h = 5;

    ballRect.x = paddleRect.x + 20;
    ballRect.y = paddleRect.y - 20;
    ballRect.w = 20;
    ballRect.h = 20;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "SDL failed to init!" << std::endl;
        SDL_Quit();
        return 1;
    }

    // load font
    if (TTF_Init() == -1)
        return 2;

    // load sdl mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
        return 3;

    // create window
    window = SDL_CreateWindow(
        "Black Art of Multiplatform Game Programming!",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

    backBuffer = SDL_GetWindowSurface(window);

    if (LoadFiles())
    {

        // play sound
        
        Mix_PlayMusic(backGroundMusic, -1);
        Mix_VolumeMusic(volume);
        while (ProgramIsRunning())
        {
            // get the time at the start of the frame
            int frameStart = SDL_GetTicks();
            // reset the back buffer with the back ground
            SDL_BlitSurface(backGroundImage, NULL, backBuffer, NULL);

            // draw the image
            paddleRect.x = (paddleRect.x + paddleRect.w < SCREEN_WIDTH + 1) ? (paddleRect.x + (inputDirectionX * movementSpeed)) : SCREEN_WIDTH - paddleRect.w;
            paddleRect.x = (paddleRect.x > 0) ? paddleRect.x : 0;

            if (isAPressed == true)
            {
                paddleRect.x = (paddleRect.x + paddleRect.w < SCREEN_WIDTH + 1) ? (ballRect.x + (ballXVel * movementSpeed)) : SCREEN_WIDTH - paddleRect.w;
                paddleRect.x = (paddleRect.x > 0) ? paddleRect.x : 0;
            }

            if (RectsOverlap(ballRect, leftRect))
            {
                ballXVel = 1.0f;
                Mix_PlayChannel(-1, hitSound, 0);
            }
            if (RectsOverlap(ballRect, rightRect))
            {
                ballXVel = -1.0f;
                Mix_PlayChannel(-1, hitSound, 0);
            }
            if (RectsOverlap(ballRect, topRect))
            {
                ballYVel = 1.0f;
                Mix_PlayChannel(-1, hitSound, 0);
            }
            if (RectsOverlap(ballRect, paddleRect))
            {
                ballYVel = -1.0f;
                Mix_PlayChannel(-1, hitSound, 0);
            }
            if (RectsOverlap(ballRect, bottomRect))
            {
                ballXVel = 0.0f;
                ballYVel = 0.0f;
                paddleRect.x = SCREEN_WIDTH / 2 - 95 / 2;
                paddleRect.y = 500;
                ballRect.x = paddleRect.x + 20;
                ballRect.y = paddleRect.y - 20;
                if (score > highscore)
                    highscore = score;
                isSpacePressed = false;
                score = 0;
            }

            ballRect.x += ballXVel * ballMovementSpeed;
            ballRect.y += ballYVel * ballMovementSpeed;

            /*ballRect.x = (ballRect.x + (ballRect.w/2.0f) < leftRect.x) ? (ballRect.x + (ballXVel * ballMovementSpeed)) : ballXVel = 1.0f;
            ballRect.x = (ballRect.x > -(ballRect.w/2.0f)) ? ballRect.x : ballXVel = -1.0f;
            ballRect.y = (ballRect.y + (ballRect.h/2.0f) < SCREEN_HEIGHT) ? (ballRect.y + (ballYVel * ballMovementSpeed)) : ballYVel = 1.0f;
            ballRect.y = (ballRect.y > -(ballRect.h/2.0f)) ? ballRect.y : ballYVel = 1.0f;*/

            DrawImage(sprite, backBuffer, ballRect.x, ballRect.y);
            DrawImage(paddleSprite, backBuffer, paddleRect.x, paddleRect.y);

            std::string scoreString = "Score: " + std::to_string((int)score);
            std::string highScoreString = "HighScore: " + std::to_string((int)highscore);

            // font
            DrawText(backBuffer, scoreString.c_str(), 25, 25, gameFont, 255u, 255u, 255u);
            DrawText(backBuffer, highScoreString.c_str(), 450, 25, gameFont, 255u, 255u, 255u);

            // end draw frame
            SDL_UpdateWindowSurface(window);

            // find the number of milliseconds
            int frameTime = SDL_GetTicks() - frameStart;
            if (isSpacePressed == true)
                score += frameTime;

            // if we are rendering faster than FPS sleep the cpu
            if (frameTime < FRAME_DELAY)
                SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    FreeFiles();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

SDL_Surface *LoadImage(const char *fileName)
{
    SDL_Surface *imageLoaded = NULL;
    SDL_Surface *processedImage = NULL;

    imageLoaded = SDL_LoadBMP(fileName);

    if (imageLoaded != NULL)
    {
        processedImage = SDL_ConvertSurface(imageLoaded, backBuffer->format, 0);
        SDL_FreeSurface(imageLoaded);

        if (processedImage != NULL)
        {
            Uint32 colorKey = SDL_MapRGB(processedImage->format, 0xFF, 0, 0xFF);
            SDL_SetColorKey(processedImage, SDL_TRUE, colorKey);
        }
    }

    return processedImage;
}

bool ProgramIsRunning()
{
    SDL_Event event;
    inputDirectionX = 0.0f;
    inputDirectionY = 0.0f;

    // input buffer
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT])
    {
        isAPressed = false;
        inputDirectionX = -1.0f;
    }

    if (keys[SDL_SCANCODE_RIGHT])
    {
        inputDirectionX = 1.0f;
        isAPressed = false;
    }

    if (keys[SDL_SCANCODE_UP])
        inputDirectionY = -1.0f;

    if (keys[SDL_SCANCODE_DOWN])
        inputDirectionY = 1.0f;

    if (keys[SDL_SCANCODE_SPACE])
    {
        isAPressed = false;
        if (isSpacePressed != true)
        {
            ballXVel = 1 + rand() % 10;
            if (ballXVel >= 1 && ballXVel <= 5)
                ballXVel = -1.0f;
            if (ballXVel >= 6 && ballXVel <= 10)
                ballXVel = 1.0f;

            ballYVel = 1 + rand() % 10;
            if (ballYVel >= 1 && ballYVel <= 5)
                ballYVel = -1.0f;
            if (ballYVel >= 6 && ballYVel <= 10)
                ballYVel = 1.0f;
            isSpacePressed = true;
        }
    }

    if (keys[SDL_SCANCODE_A])
    {
        isAPressed = true;
    }

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            return false;
        if (event.type == SDL_KEYDOWN)
        {
            /*if (event.key.keysym.sym == SDLK_LEFT)
                inputDirectionX = -1.0f;
            if (event.key.keysym.sym == SDLK_RIGHT)
                inputDirectionX = 1.0f;*/
        }
        if (event.type == SDL_MOUSEMOTION)
        {
            float x = event.motion.x;
            float y = event.motion.y;
        }
    }

    return true;
}

void DrawImage(SDL_Surface *image, SDL_Surface *destSurface, int x, int y)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;

    SDL_BlitSurface(image, NULL, destSurface, &destRect);
}

void DrawImageFrame(SDL_Surface *image, SDL_Surface *destSurface,
                    int x, int y, int width, int height, int frame)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;

    int collumns = (*image).w / width;

    SDL_Rect sourceRect;
    sourceRect.y = (frame / collumns) * height;
    sourceRect.x = (frame % collumns) * width;
    sourceRect.w = width;
    sourceRect.h = height;

    SDL_BlitSurface(image, &sourceRect, destSurface, &destRect);
}

bool LoadFiles()
{
    // load images
    backGroundImage = LoadImage("assets/graphics/greyBackground.bmp");
    sprite = LoadImage("assets/graphics/ball.bmp");
    paddleSprite = LoadImage("assets/graphics/Paddle.bmp");

    if (sprite == nullptr)
        return false;

    if (backGroundImage == nullptr)
        return false;

    // load font
    gameFont = TTF_OpenFont("assets/fonts/alfphabet.ttf", 30);

    if (gameFont == nullptr)
        return false;

    // load sounds
    hitSound = Mix_LoadWAV("assets/sounds/pingpongboard.wav");

    if (hitSound == nullptr)
        return false;

    // load music
    backGroundMusic = Mix_LoadMUS("assets/sounds/JuhaniJunkala[RetroGameMusicPack]TitleScreen.wav");

    if (backGroundMusic == nullptr)
        return false;

    return true;
}

void FreeFiles()
{
    if (sprite != nullptr)
    {
        SDL_FreeSurface(sprite);
        sprite = nullptr;
    }

    if (backGroundImage != nullptr)
    {
        SDL_FreeSurface(backGroundImage);
        backGroundImage = nullptr;
    }

    if (gameFont != nullptr)
    {
        TTF_CloseFont(gameFont);
        gameFont = nullptr;
    }

    if (hitSound != nullptr)
    {
        Mix_FreeChunk(hitSound);
        hitSound = nullptr;
    }

    if (backGroundMusic != nullptr)
    {
        Mix_FreeMusic(backGroundMusic);
        backGroundMusic = nullptr;
    }
}

void DrawText(SDL_Surface *surface, const char *string, int x, int y, TTF_Font *font, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Surface *renderedText = NULL;

    SDL_Color color;

    color.r = r;
    color.g = g;
    color.b = b;

    renderedText = TTF_RenderText_Solid(font, string, color);

    SDL_Rect pos;

    pos.x = x;
    pos.y = y;

    SDL_BlitSurface(renderedText, NULL, surface, &pos);
    SDL_FreeSurface(renderedText);
}

bool RectsOverlap(SDL_Rect rect1, SDL_Rect rect2)
{
    if (rect1.x >= rect2.x + rect2.w)
        return false;

    if (rect1.y >= rect2.y + rect2.h)
        return false;

    if (rect2.x >= rect1.x + rect1.w)
        return false;

    if (rect2.y >= rect1.y + rect1.h)
        return false;

    return true;
}
