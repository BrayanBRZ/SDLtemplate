#include "TextureManager.h"
#include "../Core/Engine.h"

TextureManager *TextureManager::s_Instance = nullptr;
float UNIT_TO_PIXELS = 1;

bool TextureManager::Load(std::string id, std::string filename)
{
    SDL_Surface *surface = IMG_Load(filename.c_str());
    if (surface == nullptr)
    {
        SDL_Log("Failed to create surface. Error: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Engine::GetInstance()->GetRenderer(), surface);
    if (texture == nullptr)
    {
        SDL_Log("Failed to create texture from surface. Error: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    m_TextureMap[id] = texture;
    SDL_FreeSurface(surface);
    return true;
}

void TextureManager::Draw(std::string id, float x, float y, int width, int heigth)
{
    SDL_Rect srcRect = {0, 0, (int)(width * UNIT_TO_PIXELS), (int)(heigth * UNIT_TO_PIXELS)};
    SDL_Rect dstRect = {(int)(x * UNIT_TO_PIXELS), (int)(y * UNIT_TO_PIXELS), (int)(width * UNIT_TO_PIXELS), (int)(heigth * UNIT_TO_PIXELS)};
    SDL_RenderCopyEx(Engine::GetInstance()->GetRenderer(), m_TextureMap[id], &srcRect, &dstRect, 0, nullptr, SDL_FLIP_NONE);
}

void TextureManager::DrawFrame(std::string id, float x, float y, float objectWidth, float objectHeigth, float spriteWidth, float spriteHeigth, int row, int frame)
{
    SDL_Rect srcRect = {(int)((spriteWidth * frame) * UNIT_TO_PIXELS), (int)((spriteHeigth * (row - 1)) * UNIT_TO_PIXELS), (int)(spriteWidth * UNIT_TO_PIXELS), (int)(spriteHeigth * UNIT_TO_PIXELS)};
    SDL_Rect dstRect = {(int)(x * UNIT_TO_PIXELS), (int)(y * UNIT_TO_PIXELS), (int)(objectWidth * UNIT_TO_PIXELS), (int)(objectHeigth * UNIT_TO_PIXELS)};
    SDL_RenderCopyEx(Engine::GetInstance()->GetRenderer(), m_TextureMap[id], &srcRect, &dstRect, 0, nullptr, SDL_FLIP_NONE);
}

void TextureManager::Drop(std::string id)
{
    SDL_DestroyTexture(m_TextureMap[id]);
    m_TextureMap.erase(id);
}

SDL_Texture *TextureManager::FindTexture(std::string id)
{
    return m_TextureMap[id];
}

void TextureManager::CleanTexture()
{
    for (auto const& [_, texture] : m_TextureMap)
    {
        SDL_DestroyTexture(texture);
    }

    m_TextureMap.clear();
    SDL_Log("Texture map cleaned");
}