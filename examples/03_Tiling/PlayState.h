/*
 *  PlayState.h
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#ifndef PLAY_STATE_H_
#define PLAY_STATE_H_

#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <tmx/MapLoader.h>
#include <string>

class PlayState : public cgf::GameState
{
    public:

    void init();
    void cleanup();

    void pause();
    void resume();

    void handleEvents(cgf::Game* game);
    void update(cgf::Game* game);
    void moveAllEnemyShots(cgf::Game* game);
    void generateEnemyShots(cgf::Game* game);
    void checkEnemiesMoves(cgf::Game* game);
    void draw(cgf::Game* game);
    void checkCollisions();
    void playerShoot(cgf::Game* game,cgf::Sprite* player, cgf::Sprite* obj);
    void enemyShoot(cgf::Game* game,cgf::Sprite* enem, cgf::Sprite* obj, int i, int j);
    void checkVictory();
    void restart();

    // Implement Singleton Pattern
    static PlayState* instance()
    {
        return &m_PlayState;
    }

    protected:

    PlayState() {}

    private:

    static PlayState m_PlayState;

    enum { RIGHT=0, LEFT, UP, DOWN };
    std::string walkStates[4];
    int currentDir;

    int x, y;
    int dirx, diry;

    cgf::Sprite player;
    cgf::Sprite shot;
    //cgf::Sprite enemy;
    cgf::Sprite enemies[10][3];
    cgf::Sprite lives_icons[3];
    cgf::Sprite enemies_shot[10][3];
    cgf::Sprite asteroids[3][3];

    sf::RenderWindow* screen;
    cgf::InputManager* im;

    sf::SoundBuffer shootSoundBuffer;
    sf::Sound shootSound;
    sf::Music music;

    // Checks collision between a sprite and a map layer
    bool checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj);

};

#endif
