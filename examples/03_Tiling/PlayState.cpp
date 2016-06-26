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
bool asteroids_alive[3][3];
float turn;
float enemies_speed;
int enemies_dead;
int lives;
int won;
int textTimer = 200;
int gracePeriod = 30;
bool drawPlayer = true;

using namespace std;

void PlayState::init()
{
    if (!font.loadFromFile("data/fonts/arial.ttf")) {
        cout << "Cannot load arial.ttf font!" << endl;
        exit(1);
    }
    text.setFont(font);
    text.setString("YOU ARE EARTH'S LAST HOPE! PROTECT IT!");
    text.setCharacterSize(24); // in pixels
    text.setColor(sf::Color::Yellow);
    text.setStyle(sf::Text::Bold);
    text.setPosition(120, 10);

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
    shot.load("data/img/bullet.png");
    shot.setPosition(400,-500);
    shot.setScale(0.5,0.5);
    shot.play();

    background.load("data/img/background.jpg");
    background.setPosition(0,0);
    background.setScale(0.25,0.25);
    background.play();

    //CARREGA OS TIROS DOS INIMIGOS
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
            enemies_shot[i][j].load("data/img/bullet_red.png");
            enemies_shot[i][j].setPosition(400,-500);
            enemies_shot[i][j].setScale(0.5, 0.5);
            enemies_shot[i][j].play();
        }

    //CARREGA ICONES DE VIDA
    for(int i = 0; i<3; i++)
    {
        lives_icons[i].loadXML("data/img/player.xml");
        lives_icons[i].setPosition(10+(i*30),10);
        lives_icons[i].setScale(0.05,0.05);
        lives_icons[i].play();
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

    int posX = 100;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            asteroids_alive[i][j] = true;
            if(j == 0)
            {
                asteroids[i][j].load("../sprites/asteroid_top.png",511,109,0,0,0,0,13,21,273);
                asteroids[i][j].setPosition(posX, 405);
                asteroids[i][j].setScale(0.2,0.2);
            }
            else if(j == 1)
            {
                asteroids[i][j].load("../sprites/asteroid_mid.png",511,136,0,0,0,0,13,21,273);
                asteroids[i][j].setPosition(posX, asteroids[i][j-1].getPosition().y + 109 * 0.2);
                asteroids[i][j].setScale(0.2,0.2);
            }
            else if(j == 2)
            {
                asteroids[i][j].load("../sprites/asteroid_bottom.png",511,145,0,0,0,0,13,21,273);
                asteroids[i][j].setPosition(posX, asteroids[i][j-1].getPosition().y + 136 * 0.2);
                asteroids[i][j].setScale(0.2,0.2);
            }
        }
        posX += 250;
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

void PlayState::restart()
{
    // from Street Fighter 2 AND Space Jam soundtrack - https://www.youtube.com/watch?v=QDFDzTOyLvo

    turn = 1.0;
    enemies_speed = 1.0;
    enemies_dead = 0;
    lives = 3;
    won = 0;

    //CARREGA O JOGADOR
    player.setPosition(400,500);

    //CARREGA O TIRO DO JOGADOR
    shot.setPosition(400,-500);

    //CARREGA ICONES DE VIDA
    for(int i = 0; i<3; i++)
        lives_icons[i].setPosition(10+(i*30),10);

    //CARREGA OS TIROS DOS INIMIGOS
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
            enemies_shot[i][j].setPosition(400,-500);
        }

    //MATRIZ DE INIMIGOS VIVOS: 0 = MORTO, 1 = VIVO
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            enemies_alive[i][j] = 1;

    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
        {
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

    int posX = 100;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            asteroids_alive[i][j] = true;
            if(j == 0)
            {
                asteroids[i][j].setPosition(posX, 405);
            }
            else if(j == 1)
            {
                asteroids[i][j].setPosition(posX, asteroids[i][j-1].getPosition().y + 109 * 0.2);
            }
            else if(j == 2)
            {
                asteroids[i][j].setPosition(posX, asteroids[i][j-1].getPosition().y + 136 * 0.2);
            }
        }
        posX += 250;
    }

    dirx = 0; // sprite dir: right (1), left (-1)
    diry = 0; // down (1), up (-1)
    shoot = false;

    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            enemies_shoot[i][j] = false;

    cout << "PlayState: Restart" << endl;
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
        obj->setPosition( px, py - tamy);

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
        obj->setPosition( px, py + tamy);

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

            if(enemies_shot[i][j].bboxCollision(player)&&gracePeriod==0)
            {
                lives--;
                gracePeriod = 50;
                player.setPosition(400,500);
            }
            if(enemies[i][j].bboxCollision(player)&&gracePeriod==0)
            {
                lives--;
                gracePeriod = 50;
                player.setPosition(400,500);
            }

            for(int k = 0; k < 3; k++)
                for(int l = 0; l < 3; l++)
                {
                    if(enemies_shot[i][j].bboxCollision(asteroids[k][l]))
                    {
                        asteroids_alive[k][l] = false;
                        asteroids[k][l].setPosition(-400, - 500);
                        enemies_shot[i][j].setPosition(-543, -300);
                    }
                }
        }

    for(int k = 0; k < 3; k++)
            for(int l = 0; l < 3; l++)
            {
                if(shot.bboxCollision(asteroids[k][l]))
                {
                    shot.setPosition(-543, -300);
                }
            }

}

void PlayState::checkVictory()
{
    if(lives == 2)
    {
        lives_icons[2].setPosition(400, -500);
    }
    if(lives == 1)
    {
        lives_icons[1].setPosition(400, -500);
    }
    if(lives == 0)
    {
        lives_icons[0].setPosition(400, -500);
    }
    if(lives == -1)
    {
       cout << "YOU LOSE\n";
       text.setString("YOU DIED!");
       textTimer = 200;
       text.setPosition(120,10);
       restart();
    }
    if(lives == -2)
    {
        cout << "YOU LOSE! EARTH IS UNDER ATTACK!\n";
        text.setString("YOU LET THE ENEMIES INVADE EARTH!");
        textTimer = 200;
        text.setPosition(120,10);
        restart();
    }
    if(won == 1)
    {
        cout << "YOU WIN\n";
        text.setString("CONGRATULATIONS! YOU SAVED EARTH!");
        textTimer = 200;
        text.setPosition(120,10);
        restart();
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
                lives = -2;
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

void drawAsteroids()
{

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

    drawAsteroids();

    if(gracePeriod>0)
    {
        gracePeriod--;
    }
    player.update(game->getUpdateInterval());
    //enemy.update(game->getUpdateInterval());

    checkEnemiesMoves(game);
    textTimer--;
    if(textTimer == 0)
    {
        text.setPosition(-400,-500);
    }
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
    screen->draw(background);
    screen->draw(shot);

    if(gracePeriod > 0)
    {
        if(gracePeriod%10==0)
        {
            drawPlayer = false;
        }
        else if(gracePeriod%5==0)
        {
            drawPlayer = true;
        }
    }
    if(drawPlayer == true)
    {
        screen->draw(player);
    }
    screen->draw(text);

    for(int i = 0; i<3; i++)
            screen->draw(lives_icons[i]);
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            screen->draw(enemies_shot[i][j]);
    //screen->draw(enemy);
    for(int i = 0; i<10; i++)
        for(int j = 0; j<3; j++)
            if(enemies_alive[i][j] == 1)
                screen->draw(enemies[i][j]);

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(asteroids_alive[i][j])
                screen->draw(asteroids[i][j]);
}


/*
bool PlayState::checkCollision(uint8_t layer, cgf::Game* game, cgf::Sprite* obj)
{
    bool bump = false;
}
*/
