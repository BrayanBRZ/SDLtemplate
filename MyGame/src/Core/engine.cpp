#include "Engine.h"
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "..\Graphics\TextureManager.h"
#include "..\Events\Event.h"
#include "..\Timer\timer.h"
#include "..\Characters\Player.h"
#include "..\Characters\Enemy.h"
#include "..\TileMap\MapParser.h"
#include "..\Camera\Camera.h"

Engine *Engine::s_Instance = nullptr;

bool Engine::Init()
{
    // Inicializa SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0 && IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != 0)
    {
        SDL_Log("Failed to initialize SDL. Error: %s", SDL_GetError());
        return false;
    }

    // Inicializa SDL_image
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        SDL_Log("Failed to initialize SDL_image. Error: %s", IMG_GetError());
        return false;
    }

    // Inicializa SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        SDL_Log("Failed to initialize SDL_mixer. Error: %s", Mix_GetError());
        return false;
    }

    // Preparando os áudios
    std::vector<std::string> soundPaths = {
        "C:/msys64/home/braya/SDLtemplate/MyGame/assets/Sounds/Minecraft Damage.mp3"
        // Adicione mais sons aqui...
    };
    for (const std::string& path : soundPaths) {
        if (!LoadSound(path)) {
            SDL_Log("Failed to load sound: %s", path.c_str());
            return false;
        }
    }

    // Cria a janela
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    m_Window = SDL_CreateWindow("MyGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
    if (m_Window == nullptr)
    {
        SDL_Log("Failed to create window. Error: %s", SDL_GetError());
        return false;
    }

    // Cria o renderer
    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED); // | SDL_RENDERER_PRESENTVSYNC
    if (m_Window == nullptr)
    {
        SDL_Log("Failed to create Renderer. Error: %s", SDL_GetError());
        return false;
    }

    // Carrega o mapa
    if (!MapParser::GetInstance()->Load("C:/msys64/home/braya/SDLtemplate/MyGame/assets/maps/map-rednew.tmx"))
    {
        std::cout << "Failed to Load Map" << '\n';
    }

    m_LevelMap = MapParser::GetInstance()->GetMap("MAP", 0, 0);

    // Cria o jogador
    m_Player = new Player(Properties{.Width = 2, .Heigth = 2, .X = 193 * DEFAULT_SCALE, .Y = 117 * DEFAULT_SCALE});
    if (!m_Player)
    {
        std::cout << "Failed to create player" << '\n';
        return false;
    }

    // Cria inimigos
    m_Enemies.push_back(new Enemy(Properties{.Width = 2, .Heigth = 2, .X = 257 * DEFAULT_SCALE, .Y = 119 * DEFAULT_SCALE}));
    if (!m_Enemies.back())
    {
        std::cout << "Failed to create enemy" << '\n';
        return false;
    }

    // Configura a câmera
    Camera::GetInstance()->SetTarget(m_Player->GetOrigin());
    Camera::GetInstance()->SetLimitPosition(m_LevelMap->GetMapPosition());
    Camera::GetInstance()->SetLimitDimension(m_LevelMap->GetMapDimension());

    return m_isRunning = true;
}

// Método para carregar som
bool Engine::LoadSound(const std::string &path)
{
    Mix_Chunk *sound = Mix_LoadWAV(path.c_str());
    if (sound == nullptr)
    {
        SDL_Log("Failed to load sound: %s, Error: %s", path.c_str(), Mix_GetError());
        return false;
    }
    m_SoundsList.push_back(sound); // Adiciona o som ao vetor
    return true;
}

// Método para tocar som
void Engine::PlaySound(int soundID)
{
    if (soundID >= 0 && soundID < m_SoundsList.size())
    {
        Mix_PlayChannel(-1, m_SoundsList[soundID], 0);
    }
    else
    {
        SDL_Log("Invalid sound ID: %d", soundID);
    }
}

void Engine::Update()
{
    float dt = Timer::GetInstance()->GetDeltaTime();

    m_Player->Update(dt);
    for (auto enemy : m_Enemies)
    {
        enemy->Update(dt);
    }

    Camera::GetInstance()->Update(dt);
}

void Engine::Render()
{
    SDL_SetRenderDrawColor(m_Renderer, 124, 218, 254, 255);
    SDL_RenderClear(m_Renderer);

    m_LevelMap->Render();
    m_Player->Draw();
    for (auto enemy : m_Enemies)
    {
        enemy->Draw();
    }

    SDL_RenderPresent(m_Renderer);
}

void Engine::Events()
{
    Event::GetInstance()->Listen();
}

bool Engine::CleanGame()
{

    MapParser::GetInstance()->Clean();
    TextureManager::GetInstance()->CleanTexture();

    for (Mix_Chunk *sound : m_SoundsList)
    { Mix_FreeChunk(sound); }
    m_SoundsList.clear();

    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);

    IMG_Quit();
    SDL_Quit();
    return true;
}

void Engine::Quit()
{
    m_isRunning = false;
}
