#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SDL.h>
#include <SDL_image.h>
#include "..\Global\GlobalProperties.h"
#include "..\TileMap\GameMap.h"

class Engine
{
public:
    static Engine* GetInstance(){
        return s_Instance = (s_Instance != nullptr)? s_Instance : new Engine();
    }

    bool Init();
    bool CleanGame();
    void Quit();

    void Update();
    void Render();
    void Events();

    inline bool isRunning() {return m_isRunning;}
    inline SDL_Renderer* GetRenderer() {return m_Renderer;}

private:
    Engine() = default;
    bool m_isRunning;

    GameMap *m_LevelMap;
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    static Engine* s_Instance;
};

#endif  // __ENGINE_H__