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

    //CARREGA O JOGADOR
    player.loadXML("data/img/player.xml");
    player.setPosition(400,500);
    player.loadAnimation("data/img/playeranim.xml");
    player.setAnimation("fly");
    player.setAnimRate(10);
    player.setScale(0.1,0.1);
    player.play();

    //CARREGA O TIRO DO JOGADOR
    shot.load("data/img/shot.png",8,23,0,0,0,0,13,21,273);
    shot.setPosition(400,-500);
    shot.loadAnimation("data/img/warrioranim.xml");
    shot.setAnimation(walkStates[currentDir]);
    shot.setAnimRate(30);
    shot.setScale(1,1);
    shot.play();

    //CARREGA UM INIMIGO PLACEHOLDER PARA TESTES
    enemy.load("data/img/enemy.png",41,35,0,0,0,0,13,21,273);
    enemy.setPosition(400,100);
    enemy.setScale(1,1);
    enemy.play();

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
    if(shoot == true)
    {
        float px = obj->getPosition().x;
        float py = obj->getPosition().y;

        double deltaTime = game->getUpdateInterval();

        sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

        float vx = offset.x;
        float vy = offset.y;

        cout << "Py: " << py << " vy: " << vy << " tam: " << obj->getScale().y << "\n";

        if (py - 10 <= 0 - 50) //mudar constante pelo tamanho do sprite real
        {
            shoot = false;
        }

        obj->setPosition(px,py-10); // 10 é a velocidade do tiro, será mudada
    }
}


void playerShoot(cgf::Game* game,cgf::Sprite* player, cgf::Sprite* obj)
{
    if (!shoot)
    {
        float px = player->getPosition().x;
        float py = player->getPosition().y;
        float tamy = player->getSize().y * player->getScale().y;
        float tamx = player->getSize().x * player->getScale().x;
        obj->setPosition( px + tamx/2 , py - tamy);

        shoot = true;
     }
}

void PlayState::checkCollisions()
{
    //Quando tivermos mais inimigos, basta iterar por eles
    if(shot.bboxCollision(enemy))
    {
        enemy.setPosition(300, -500); //fora da tela
        shoot = false;
        shot.setPosition(400, -500); //fora da tela
    }
}

void PlayState::update(cgf::Game* game)
{
    screen = game->getScreen();
    //checkCollision(2, game, &player);
    movePlayer(game, &player);
    moveShot(game, &shot);



    checkCollisions();

    player.update(game->getUpdateInterval());

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
        playerShoot(game, &player, &shot);
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
    }

    player.setXspeed(500*dirx);
    player.setYspeed(1000*diry);

    shot.setXspeed(500*dirx);
    shot.setYspeed(10000*diry);
    player.play();
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    screen->draw(player);
    screen->draw(shot);
    screen->draw(enemy);
}


/*
bool PlayState::checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj)
{
    bool bump = false;
}
*/
