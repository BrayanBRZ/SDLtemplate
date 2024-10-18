#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>  // Adicionado para uso do SDL_mixer
#include <vector>
#include "..\Global\GlobalProperties.h"
#include "..\TileMap\GameMap.h"
#include "..\Characters\Player.h"
#include "..\Characters\Enemy.h"

class Engine
{
public:
    static Engine* GetInstance() {
        return s_Instance = (s_Instance != nullptr) ? s_Instance : new Engine();
    }

    bool Init();
    bool CleanGame();
    void Quit();

    void Update();
    void Render();
    void Events();

    inline GameMap* GetMap() { return m_LevelMap; }
    inline bool isRunning() { return m_isRunning; }
    inline SDL_Renderer* GetRenderer() { return m_Renderer; }
    Player* GetPlayer() { return m_Player; }
    std::vector<Enemy*>& GetEnemies() { return m_Enemies; }

    // Métodos para lidar com áudio
    bool LoadSound(const std::string& path);  // Carrega um som no vetor
    void PlaySound(int soundID);  // Toca um som pelo índice no vetor
    
private:
    Engine() = default;

    bool m_isRunning;
    GameMap* m_LevelMap;
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    static Engine* s_Instance;

    Player* m_Player;
    std::vector<Enemy*> m_Enemies;
    std::vector<Mix_Chunk*> m_SoundsList;
};

#endif  // __ENGINE_H__
