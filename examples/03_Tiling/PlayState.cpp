/*
 *  PlayState.cpp
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#include <iostream>
#include <cmath>
#include "Game.h"
#include "PlayState.h"
#include "InputManager.h"

#define XSCREENSIZE 800
#define YPLAYER 500
#define PLAYERSIZE 50

PlayState PlayState::m_PlayState;
bool shoot;

using namespace std;



void PlayState::init()
{

    walkStates[0] = "walk-right";
    walkStates[1] = "walk-left";
    walkStates[2] = "walk-up";
    walkStates[3] = "walk-down";
    currentDir = RIGHT;
    cout << "alo\n";
    player.load("data/img/warrior.png",64,64,0,0,0,0,13,21,273);
    player.setPosition(400,500);
    cout << "alo\n";
    player.loadAnimation("data/img/warrioranim.xml");
    player.setAnimation(walkStates[currentDir]);
    player.setAnimRate(30);
    player.setScale(1,1);
    player.play();

    shot.load("data/img/warrior.png",64,64,0,0,0,0,13,21,273);
    //shot.setPosition(400,500);
    shot.loadAnimation("data/img/warrioranim.xml");
    shot.setAnimation(walkStates[currentDir]);
    shot.setAnimRate(30);
    shot.setScale(1,1);
    shot.play();

    cout << "alo\n";

    dirx = 0; // sprite dir: right (1), left (-1)
    diry = 0; // down (1), up (-1)
    shoot = false;


    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("up", sf::Keyboard::Up);
    im->addKeyInput("space", sf::Keyboard::Space);
    im->addKeyInput("down", sf::Keyboard::Down);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("stats", sf::Keyboard::S);
    im->addMouseInput("rightclick", sf::Mouse::Right);

    // Camera control
    im->addKeyInput("zoomin", sf::Keyboard::Z);
    im->addKeyInput("zoomout", sf::Keyboard::X);

    cout << "PlayState: Init" << endl;
}

void PlayState::cleanup()
{
    cout << "PlayState: Clean" << endl;
}

void PlayState::pause()
{
    cout << "PlayState: Paused" << endl;
}

void PlayState::resume()
{
    cout << "PlayState: Resumed" << endl;
}

void PlayState::handleEvents(cgf::Game* game)
{
    screen = game->getScreen();
    sf::View view = screen->getView(); // gets the view
    sf::Event event;

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
    }

    dirx = diry = 0;
    shoot = false;
    int newDir = currentDir;

    if(im->testEvent("left")) {
        dirx = -1;
        newDir = LEFT;
    }

    if(im->testEvent("right")) {
        dirx = 1;
        newDir = RIGHT;
    }


    if(im->testEvent("space")) {
        shoot = true;
    }

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("stats"))
        game->toggleStats();

    if(dirx == 0 && diry == 0) {
        player.pause();
    }
    else {
        if(currentDir != newDir) {
            currentDir = newDir;
        }
        player.play();
    }

    player.setXspeed(500*dirx);
    player.setYspeed(1000*diry);

    shot.setXspeed(500*dirx);
    shot.setYspeed(10000*diry);
}

bool movePlayer(cgf::Game* game, cgf::Sprite* obj)
{
    float px = obj->getPosition().x;

    double deltaTime = game->getUpdateInterval();

    sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

    float vx = offset.x;

    cout << "Px: " << px << " vx: " << vx << " tam: " << obj->getScale().x << "\n";

    if(px + vx + PLAYERSIZE >= XSCREENSIZE)
    {
        px = XSCREENSIZE - PLAYERSIZE;
        vx = 0;
    }
    else if(px + vx <= 0)
    {
        px = 0;
        vx = 0;
    }


    obj->setPosition(px+vx,YPLAYER);

}

bool moveShot(cgf::Game* game, cgf::Sprite* obj)
{
    if(shoot == false)
    {
        float px = obj->getPosition().x;
        float py = obj->getPosition().y;

        double deltaTime = game->getUpdateInterval();

        sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

        float vx = offset.x;
        float vy = offset.y;


        cout << "Py: " << py << " vy: " << vy << " tam: " << obj->getScale().y << "\n";


        obj->setPosition(px,py-10);
    }


}


void playerShoot(cgf::Game* game,cgf::Sprite* player, cgf::Sprite* obj)
{
    if (shoot)
    {


        float px = player->getPosition().x;
        float py = player->getPosition().y;
        float tam = player->getSize().y;
        obj->setPosition( px , py - tam);

        shoot = false;
     }
}

void PlayState::update(cgf::Game* game)
{
    screen = game->getScreen();
    //checkCollision(2, game, &player);


    movePlayer(game, &player);
    moveShot(game, &shot);

        playerShoot(game, &player,&shot);




//    player.update(game->getUpdateInterval());
    //centerMapOnPlayer();

}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    screen->draw(player);
    screen->draw(shot);
}



bool PlayState::checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj)
{
    bool bump = false;
}
