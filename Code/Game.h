#pragma once
#ifndef Game_H
#define Game_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameMap.h"
#include "Enemy.h"
#include "Tower.h"
#include "Bullet.h"

const int MAX_TOWERS = 50;
const int MAX_BULLETS = 600;

// Top-level game states
enum GameState {
    STATE_MAP_SELECT,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_WIN
};

class Game {
private:
    RenderWindow window;

    // Owned heap objects (null until resetGame is called)
    GameMap* map;
    Enemy** enemies;
    Tower** towers;
    Bullet** bullets;
    Tower* selectedTower;
    int enemyCount, towerCount, bulletCount;

    int gold, lives, wave;
    int score, highScore;
    int mapHighScores[3];   // cached best scores for each map (shown on select screen)

    GameState gameState;
    int       selectedMapIndex;

    sf::Font font;
    sf::Text hudText;

    // --- Sound buffers (shared by multiple sound instances) ---
    sf::SoundBuffer cannonBuffer, sniperBuffer, machineBuffer,
        enemyFireBuffer, towerDestroyBuffer,
        enemyLeaveBuffer, towerPlaceBuffer,
        winBuffer, loseBuffer;

    // --- Sound players (one per type; multiple can play overlapping) ---
    sf::Sound cannonSound, sniperSound, machineSound,
        enemyFireSound, towerDestroySound,
        enemyLeaveSound, towerPlaceSound,
        winSound, loseSound;

    // --- Background music (streamed) ---
    sf::Music bgMusic;

    float spawnTimer, spawnInterval;
    int   enemiesToSpawn, enemiesSpawned;
    int  selectedCol, selectedRow;

    // For shop UI
    bool showShopDetails;
    int  shopSelectedIndex;   // which tower is highlighted (0..4)
    struct TowerInfo {
        const char* name;
        int cost;
        float range;
        float damage;
        float fireRate;       // shots per second?
        const char* desc;
    };
    TowerInfo towerInfos[5];   // populated in constructor

public:
    Game();
    ~Game();
    void run();

private:
    // Main loop
    void handleInput();
    void update(float dt);
    void render();

    // Gameplay
    void spawnEnemy();
    void updateEnemyAttacks();          // enemies stop & shoot towers
    void checkBulletCollisions();       // bullets hit enemies or towers
    void checkEnemiesReachedEnd();
    void cleanupDead();
    void cleanupDeadTowers();
    void cleanupBullets();
    void startWave(int waveNum);

    // UI
    void drawHUD();
    void renderMapSelect();
    void handleMapSelectInput(int mx, int my);


    // Session management
    void resetGame(int mapIdx);
    void calculateScore();

    // High score file I/O
    int  loadHighScore(int mapIdx);
    void saveHighScore(int mapIdx, int hs);
    void loadAllHighScores();
};

#endif