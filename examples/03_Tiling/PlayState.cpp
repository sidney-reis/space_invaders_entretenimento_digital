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
bool enemies_shoot[10][3];
int enemies_alive[10][3];
float turn;
float enemies_speed;
int enemies_dead;
int lives;
int won;

using namespace std;

void PlayState::init()
{
    // from Street Fighter 2 AND Space Jam soundtrack - https://www.youtube.com/watch?v=QDFDzTOyLvo
    music.openFromFile("data/nost/Guile_Theme_-_Slam_Jam_Remix.ogg");
    music.setVolume(30);  // 30% of max. voume
    music.setLoop(true);  // loop mode
    music.play();

    turn = 1.0;
    enemies_speed = 1.0;
    enemies_dead = 0;
    lives = 3;
    won = 0;

    shootSoundBuffer.loadFromFile("data/sounds/shoot.wav");
    shootSound.setBuffer(shootSoundBuffer);
    shootSound.setAttenuation(0);

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
    shot.setScale(1,1);
    shot.play();

    //CARREGA OS TIROS DOS INIMIGOS
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
            enemies_shot[i][j].load("data/img/shot.png",8,23,0,0,0,0,13,21,273);
            enemies_shot[i][j].setPosition(400,-500);
            enemies_shot[i][j].setScale(1,1);
            enemies_shot[i][j].play();
        }

    //MATRIZ DE INIMIGOS VIVOS: 0 = MORTO, 1 = VIVO
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            enemies_alive[i][j] = 1;

    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
            enemies[i][j].loadXML("data/img/enemy.xml");
            enemies[i][j].setPosition(400,100);
            enemies[i][j].loadAnimation("data/img/enemyanim.xml");
            enemies[i][j].setAnimation("fly");
            enemies[i][j].setAnimRate(10);
            enemies[i][j].setScale(0.15,0.15);
            enemies[i][j].play();
            enemies[i][j].setPosition((i+1)*65,(j+1)*65);
        }

    //CARREGA UM INIMIGO PLACEHOLDER PARA TESTES
    /*enemy.loadXML("data/img/enemy.xml");
    enemy.setPosition(400,100);
    enemy.loadAnimation("data/img/enemyanim.xml");
    enemy.setAnimation("fly");
    enemy.setAnimRate(10);
    enemy.setScale(0.2,0.2);
    enemy.play();
*/

    dirx = 0; // sprite dir: right (1), left (-1)
    diry = 0; // down (1), up (-1)
    shoot = false;

    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            enemies_shoot[i][j] = false;

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

    //cout << "Px: " << px << " vx: " << vx << " tam: " << obj->getScale().x << "\n";

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

        //cout << "Py: " << py << " vy: " << vy << " tam: " << obj->getScale().y << "\n";

        if (py - 10 <= 0 - 50) //mudar constante pelo tamanho do sprite real
        {
            shoot = false;
        }

        obj->setPosition(px,py-10); // 10 é a velocidade do tiro, será mudada
    }
}

bool moveEnemyShot(cgf::Game* game, cgf::Sprite* obj, int i, int j)
{
    if(enemies_shoot[i][j] == true)
    {
        float px = obj->getPosition().x;
        float py = obj->getPosition().y;

        double deltaTime = game->getUpdateInterval();

        sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

        float vx = offset.x;
        float vy = offset.y;

        //cout << "Py: " << py << " vy: " << vy << " tam: " << obj->getScale().y << "\n";

        if (py + 10 >= 600 + 50) //mudar constante pelo tamanho do sprite real
        {
            enemies_shoot[i][j] = false;
        }

        obj->setPosition(px,py+10); // 10 é a velocidade do tiro, será mudada
    }
}

bool moveEnemies(cgf::Game* game, cgf::Sprite* obj)
{
    float px = obj->getPosition().x;
    float py = obj->getPosition().y;
    double deltaTime = game->getUpdateInterval();

    sf::Vector2f offset(obj->getXspeed()/1000 * deltaTime, obj->getYspeed()/1000 * deltaTime);

    float vx = offset.x;
    float vy = offset.y;

    obj->setPosition(enemies_speed*turn + px,py);
}

void PlayState::playerShoot(cgf::Game* game,cgf::Sprite* player, cgf::Sprite* obj)
{
    if (!shoot)
    {
        shootSound.play();
        float px = player->getPosition().x;
        float py = player->getPosition().y;
        float tamy = player->getSize().y * player->getScale().y;
        float tamx = player->getSize().x * player->getScale().x;
        obj->setPosition( px + tamx/2 , py - tamy);

        shoot = true;
     }
}

void PlayState::enemyShoot(cgf::Game* game,cgf::Sprite* enem, cgf::Sprite* obj, int i, int j)
{
    if (!enemies_shoot[i][j])
    {
        float px = enem->getPosition().x;
        float py = enem->getPosition().y;
        float tamy = enem->getSize().y * enem->getScale().y;
        float tamx = enem->getSize().x * enem->getScale().x;
        obj->setPosition( px + tamx/2 , py + tamy);

        enemies_shoot[i][j] = true;
     }
}

void PlayState::checkCollisions()
{

    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
            if(shot.bboxCollision(enemies[i][j]))
            {
                if(enemies_alive[i][j] == 1)
                {
                    enemies[i][j].setPosition(300, -500); //fora da tela
                    shoot = false;
                    shot.setPosition(400, -500); //fora da tela
                    enemies_alive[i][j] = 0;
                    enemies_dead++;
                    if(enemies_dead == 5)
                        enemies_speed = 2;
                    if(enemies_dead == 15)
                        enemies_speed = 3;
                    if(enemies_dead == 20)
                        enemies_speed = 4;
                    if(enemies_dead == 25)
                        enemies_speed = 5;
                    if(enemies_dead == 29)
                        enemies_speed = 25;
                    if(enemies_dead == 30)
                        won = 1;
                }
            }
            if(enemies_shot[i][j].bboxCollision(player))
            {
                lives--;
                player.setPosition(400,500);
            }
            if(enemies[i][j].bboxCollision(player))
            {
                lives--;
                player.setPosition(400,500);
            }
        }
}

void checkVictory()
{
    if(lives == 0)
    {
        cout << "YOU LOSE\n";
    }
    if(lives == -1)
    {
        cout << "YOU LOSE! EARTH IS UNDER ATTACK!\n";
    }
    if(won == 1)
    {
        cout << "YOU WIN\n";
    }
}

void PlayState::generateEnemyShots(cgf::Game* game)
{
    int random_shot;
    bool clear_path = 1;
    int j2 = 0;
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
            if(enemies_alive[i][j] == 1)
            {
                j2 = j+1;
                clear_path = 1;
                while(j2 < 3)
                {
                    if((enemies_alive[i][j2] == 1) && (clear_path == 1))
                    {
                        clear_path = 0;
                    }
                    j2++;
                }
                if(clear_path)
                {
                    random_shot = rand() % (250 - enemies_dead*5) + 1;
                    if(random_shot == 5)
                    {
                        enemyShoot(game, &enemies[i][j], &enemies_shot[i][j], i, j);
                    }
                }
            }
        }
}

void PlayState::moveAllEnemyShots(cgf::Game* game)
{
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            moveEnemyShot(game, &enemies_shot[i][j], i, j);
}

void PlayState::checkEnemiesMoves(cgf::Game* game)
{
    float px;
    float py;
    bool touchedWall = false;
    for(int i = 0; i<10; i++)
    {
        for(int j = 0; j<3; j++)
        {
            enemies[i][j].update(game->getUpdateInterval());
            moveEnemies(game, &enemies[i][j]);
            if(enemies_alive[i][j] && (enemies[i][j].getPosition().x+65 > 800 || enemies[i][j].getPosition().x < 0))
            {
                touchedWall = true;
            }
            if(enemies_alive[i][j] && (enemies[i][j].getPosition().y > 600))
            {
                lives = -1;
            }
        }
    }

    if(touchedWall)
    {
        turn = -turn;

        for(int i = 0; i<10; i++)
            for(int j = 0; j<3; j++)
            {
                if(enemies_alive[i][j] == 1)
                {
                    px = enemies[i][j].getPosition().x;
                    py = enemies[i][j].getPosition().y + 5.0;
                    enemies[i][j].setPosition(px, py);
                }
            }
    }
}

void PlayState::update(cgf::Game* game)
{
    checkVictory();
    screen = game->getScreen();
    //checkCollision(2, game, &player);
    generateEnemyShots(game);
    movePlayer(game, &player);
    moveShot(game, &shot);
    moveAllEnemyShots(game);
    checkCollisions();



    player.update(game->getUpdateInterval());
    //enemy.update(game->getUpdateInterval());

    checkEnemiesMoves(game);
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
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
            enemies_shot[i][j].setXspeed(500*dirx);
            enemies_shot[i][j].setYspeed(10000*diry);
        }
    player.play();
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            if(enemies_alive[i][j] == 1)
                enemies[i][j].play();
    //enemy.play();
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    screen->draw(player);
    screen->draw(shot);
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            screen->draw(enemies_shot[i][j]);
    //screen->draw(enemy);
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            if(enemies_alive[i][j] == 1)
                screen->draw(enemies[i][j]);
}


/*
bool PlayState::checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj)
{
    bool bump = false;
}
*/
