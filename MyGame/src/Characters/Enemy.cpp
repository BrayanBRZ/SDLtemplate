#include "Enemy.h"
#include "../Core/engine.h"
#include "../Graphics/textureManager.h"
#include <SDL.h>

Enemy::Enemy(const Properties &props) : Character(props)
{
    m_TextureID = TextureManager::GetInstance()->Load("../assets/enemy/green.png");

    m_Collider.SetBuffer(-0.7f * DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE,
                         -1.0f * DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE,
                         1.4f * DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE,
                         1.0f * DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE);

    m_RigidBody.SetMaxSpeed(1.0f * DEFAULT_SCALE);
    m_RigidBody.ApplyVelocity(Vector(0.0f, 0.0f));
    m_knockDirection = {0.0f, 0.0f};
    m_KnockbackForce = (50.0f * DEFAULT_SCALE);
    m_KnockbackTime = 0.0f;

    m_Direction = Direction_DOWN;
}

Enemy::~Enemy()
{
    TextureManager::GetInstance()->CleanTexture();
}

void Enemy::Draw()
{
    m_Animation.Draw(m_Position.x, m_Position.y, m_Width, m_Height);

    // Desenho da colisão do aggro
    TextureManager::GetInstance()->DrawBox((int)m_AggroArea.Get().x,
                                           (int)m_AggroArea.Get().y,
                                           (int)m_AggroArea.Get().w,
                                           (int)m_AggroArea.Get().z);

    // Desenho da colisão do corpo
    TextureManager::GetInstance()->DrawBox((int)m_Collider.Get().x,
                                           (int)m_Collider.Get().y,
                                           (int)m_Collider.Get().w,
                                           (int)m_Collider.Get().z);
}

void Enemy::Update(float dt)
{
    if (m_KnockbackTime > 0)
    {
        m_KnockbackTime -= dt;
    }
    else
    {
        Enemy::Movement();
    }

    m_RigidBody.Update(dt, m_Position);
    m_Collider.Set(m_Position.x, m_Position.y, m_Width, m_Height);

    // Atualizando a área de aggro para ser centralizada
    float aggroWidth = DEFAULT_UNIT_TO_PIXELS * 0.5; // Escala o tamanho da área de aggro em largura
    float aggroHeight = DEFAULT_UNIT_TO_PIXELS * 0.5; // Escala o tamanho da área de aggro em altura

    std::cout << aggroWidth << std::endl;

    // Calcula a posição centralizada da área de aggro em torno do inimigo
    float aggroX = m_Origin->x - DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE * (aggroWidth / 2); // Centro do inimigo menos metade da largura da área de aggro
    float aggroY = m_Origin->y - DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE * (aggroHeight / 2); // Centro do inimigo menos metade da altura da área de aggro

    std::cout << DEFAULT_UNIT_TO_PIXELS * (aggroWidth / 2) << std::endl;

    // Define a área de aggro centralizada
    m_AggroArea.Set(aggroX, aggroY, aggroWidth, aggroHeight);

    m_Origin->x = m_Position.x + (m_Width / 2) * DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE;
    m_Origin->y = m_Position.y + (m_Height / 2) * DEFAULT_UNIT_TO_PIXELS * DEFAULT_SCALE;

    CollisionHandler::GetInstance()->MapCollision(m_Collider.Get(), m_Position);

    auto player = Engine::GetInstance()->GetPlayer();
    if (CollisionHandler::GetInstance()->CheckCollision(m_AggroArea.Get(), player->GetCollider().Get()))
    {
        std::cout << "Aggro Collision with Player!" << std::endl;
        m_IsAggro = true;
    }
    else
    {
        m_IsAggro = false;
    }

    // Verifique a colisão do corpo com o player
    if (CollisionHandler::GetInstance()->CheckCollision(m_Collider.Get(), player->GetCollider().Get()))
    {
        std::cout << "Body Collision with Player!" << std::endl;
        Enemy::StandByState();
        m_KnockbackTime = 1.0f;
    }
    // std::cout << "Enemy position: " << m_Position << std::endl;
    m_Animation.Update();
}

void Enemy::StandByState()
{
    if (this->m_Direction == Direction_UP)
    {
        m_Animation.SetProps(m_TextureID, 4, 1, 200, 64, 64);
    }

    if (this->m_Direction == Direction_DOWN)
    {
        m_Animation.SetProps(m_TextureID, 1, 1, 200, 64, 64);
    }

    if (this->m_Direction == Direction_LEFT)
    {
        m_Animation.SetProps(m_TextureID, 2, 1, 200, 64, 64);
    }

    if (this->m_Direction == Direction_RIGHT)
    {
        m_Animation.SetProps(m_TextureID, 3, 1, 200, 64, 64);
    }
}

void Enemy::Movement()
{
    // Stand By
    Enemy::StandByState();

    Vector target = *Engine::GetInstance()->GetPlayer()->GetOrigin();

    // Animation
    if (target.y < Enemy::GetOrigin()->y - 3.0f && m_IsAggro == true)
    {
        if (!(Event::GetInstance()->GetKeyDown(SDL_SCANCODE_S)))
        {
            m_Direction = Direction_UP;
            m_Animation.SetProps(m_TextureID, 4, 4, 200, 64, 64);
        }
        m_RigidBody.ApplyVelocityY(1.0f * DEFAULT_SCALE * UPWARD);
    }

    if (target.y > Enemy::GetOrigin()->y + 3.0f && m_IsAggro == true)
    {
        if (!(Event::GetInstance()->GetKeyDown(SDL_SCANCODE_W)))
        {
            m_Direction = Direction_DOWN;
            m_Animation.SetProps(m_TextureID, 1, 4, 200, 64, 64);
        }
        m_RigidBody.ApplyVelocityY(1.0f * DEFAULT_SCALE * DOWNWARD);
    }

    if (target.x < Enemy::GetOrigin()->x - 3.0f && m_IsAggro == true)
    {
        if (!(Event::GetInstance()->GetKeyDown(SDL_SCANCODE_D)))
        {
            m_Direction = Direction_LEFT;
            m_Animation.SetProps(m_TextureID, 2, 4, 200, 64, 64);
        }
        m_RigidBody.ApplyVelocityX(1.0f * DEFAULT_SCALE * LEFTWARD);
    }

    if (target.x > Enemy::GetOrigin()->x + 3.0f && m_IsAggro == true)
    {
        if (!(Event::GetInstance()->GetKeyDown(SDL_SCANCODE_A)))
        {
            m_Direction = Direction_RIGHT;
            m_Animation.SetProps(m_TextureID, 3, 4, 200, 64, 64);
        }
        m_RigidBody.ApplyVelocityX(1.0f * DEFAULT_SCALE * RIGHTWARD);
    }
}
