#include "Game.h"
#include "Enemy.h"
#include "BasicEnemy.h"
#include "FastEnemy.h"
#include "TankEnemy.h"
#include "FlyingEnemy.h"
#include "CannonTower.h"
#include "SniperTower.h"
#include "MachineGunTower.h"
#include "SlowTower.h"
#include"AbdTower.h"
#include"ShieldedEnemy.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <SFML/Audio.hpp>

// -----------------------------------------------------------------------
// Construction / Destruction
// -----------------------------------------------------------------------

Game::Game()
    : window(sf::VideoMode(1000, 600), "Tower Defense"),
    map(nullptr), enemies(nullptr), towers(nullptr), bullets(nullptr),
    enemyCount(0), towerCount(0), bulletCount(0),
    gold(200), lives(10), wave(1),
    score(0), highScore(0),
    selectedTower(nullptr),
    gameState(STATE_MAP_SELECT), selectedMapIndex(1),
    spawnTimer(0.f), spawnInterval(1.5f),
    enemiesToSpawn(0), enemiesSpawned(0),
    showShopDetails(false), selectedCol(0), selectedRow(0)
{
    window.setFramerateLimit(60);
    font.loadFromFile("C:/Users/hp/source/repos/Chess/Chess/Font/arial.ttf");
    hudText.setFont(font);
    hudText.setCharacterSize(18);
    hudText.setFillColor(sf::Color::White);
    hudText.setPosition(10, 10);
    towerInfos[0] = { "Cannon", 100, 120.f, 50.f, 1.0f, "Heavy damage, slow fire" };
    towerInfos[1] = { "Sniper", 120, 200.f, 80.f, 2.0f, "Long range, precise shot" };
    towerInfos[2] = { "MachineGun", 180, 100.f, 20.f, 5.0f, "Fast fire, low damage" };
    towerInfos[3] = { "Slow", 200, 120.f, 0.f, 1.5f, "Slows enemies in area" };
    towerInfos[4] = { "Buff Tower", 150, 90.f, 0.f, 2.0f, "Boosts nearby towers" };
    showShopDetails = false;
    shopSelectedIndex = 0;

    // ----- Load all sound buffers -----
    const std::string path = "C:/Users/hp/source/repos/Chess/Chess/Sounds/";

    cannonBuffer.loadFromFile(path + "cannon.wav");
    sniperBuffer.loadFromFile(path + "sniperfire.wav");
    machineBuffer.loadFromFile(path + "machine.wav");
    enemyFireBuffer.loadFromFile(path + "Enemyfiring.wav");
    towerDestroyBuffer.loadFromFile(path + "faaaa.wav");
    enemyLeaveBuffer.loadFromFile(path + "enemyLeave.wav");
    towerPlaceBuffer.loadFromFile(path + "Towerplacing.wav");
    winBuffer.loadFromFile(path + "Won.wav");
    loseBuffer.loadFromFile(path + "lose.wav");

    // Set buffers to sounds
    cannonSound.setBuffer(cannonBuffer);
    sniperSound.setBuffer(sniperBuffer);
    machineSound.setBuffer(machineBuffer);
    enemyFireSound.setBuffer(enemyFireBuffer);
    towerDestroySound.setBuffer(towerDestroyBuffer);
    enemyLeaveSound.setBuffer(enemyLeaveBuffer);
    towerPlaceSound.setBuffer(towerPlaceBuffer);
    winSound.setBuffer(winBuffer);
    loseSound.setBuffer(loseBuffer);

    // Background music (use .wav or .mp3, sf::Music handles both)
   /* if (bgMusic.openFromFile(path + "bgmusic.wav"))
    {
        bgMusic.setLoop(true);
        bgMusic.play();
    }*/

    loadAllHighScores();
}

Game::~Game() {
    bgMusic.stop();
    if (enemies) { for (int i = 0; i < enemyCount; i++) delete enemies[i]; delete[] enemies; }
    if (towers) { for (int i = 0; i < towerCount; i++) delete towers[i];  delete[] towers; }
    if (bullets) { for (int i = 0; i < bulletCount; i++) delete bullets[i]; delete[] bullets; }
    if (map)     delete map;
}

// -----------------------------------------------------------------------
// Session management
// -----------------------------------------------------------------------

void Game::resetGame(int mapIdx) {
    // Clean up previous session
    if (enemies) { for (int i = 0; i < enemyCount; i++) delete enemies[i]; delete[] enemies; enemies = nullptr; }
    if (towers) { for (int i = 0; i < towerCount; i++) delete towers[i];  delete[] towers;  towers = nullptr; }
    if (bullets) { for (int i = 0; i < bulletCount; i++) delete bullets[i]; delete[] bullets; bullets = nullptr; }
    if (map) { delete map; map = nullptr; }
    if (selectedTower)
    {
        delete selectedTower;
        selectedTower = nullptr;
    }
    

    selectedMapIndex = mapIdx;
    map = new GameMap(mapIdx);
    enemies = new Enemy * [MAX_ENEMIES];
    towers = new Tower * [MAX_TOWERS];
    bullets = new Bullet * [MAX_BULLETS];

    enemyCount = 0;
    towerCount = 0;
    bulletCount = 0;
    gold = 200;
    lives = 10;
    wave = 1;
    score = 0;
    highScore = loadHighScore(mapIdx);
    showShopDetails = false;
    gameState = STATE_PLAYING;

    startWave(1);
}

void Game::calculateScore() {
    score = gold + lives * 50 + (wave - 1) * 100;
    if (gameState == STATE_WIN) score += 500;
    if (score > highScore) {
        highScore = score;
        saveHighScore(selectedMapIndex, highScore);
        mapHighScores[selectedMapIndex - 1] = highScore;
    }
}

// -----------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleInput();
        update(dt);
        render();
    }
}

void Game::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        if (showShopDetails && event.type == sf::Event::MouseButtonPressed) {
            int mx = event.mouseButton.x;
            int my = event.mouseButton.y;

            if (event.mouseButton.button == sf::Mouse::Right) {
                showShopDetails = false;
                selectedTower = nullptr;
                return;
            }

            // ---------- BUY MODE (selectedTower == nullptr) ----------
            if (selectedTower == nullptr) {
                for (int i = 0; i < 5; i++) {
                    sf::IntRect row(830, 100 + i * 58, 280, 50);
                    if (row.contains(mx, my)) {
                        shopSelectedIndex = i;
                        float px = selectedCol * TILE + TILE / 2.f;
                        float py = selectedRow * TILE + TILE / 2.f;
                        int cost = towerInfos[i].cost;

                        if (gold >= cost && towerCount < MAX_TOWERS) {
                            switch (i) {
                            case 0: towers[towerCount++] = new CannonTower(px, py); break;
                            case 1: towers[towerCount++] = new snipertower(px, py); break;
                            case 2: towers[towerCount++] = new machineguntower(px, py); break;
                            case 3: towers[towerCount++] = new slowtower(px, py); break;
                            case 4: towers[towerCount++] = new AbdTower(px, py); break;
                            }
                            gold -= cost;
                            map->setBlocked(selectedCol, selectedRow);
                            towerPlaceSound.play();
                        }
                        showShopDetails = false;
                        return;
                    }
                }
            }

            // ---------- UPGRADE MODE (selectedTower != nullptr) ----------
            if (selectedTower != nullptr) {
                sf::IntRect upgradeBtn(850, 200, 200, 40);   // matches rendered Upgrade button
                if (upgradeBtn.contains(mx, my)) {
                    if (selectedTower->canUpgrade() && gold >= selectedTower->getUpgradeCost()) {
                        gold -= selectedTower->getUpgradeCost();
                        selectedTower->upgrade();
                        // optional: play a sound here (e.g., towerPlaceSound or a new one)
                    }
                    showShopDetails = false;
                    selectedTower = nullptr;
                    return;
                }
            }

            // ----- Clicked outside any interactive element -----
            showShopDetails = false;
            selectedTower = nullptr;
            return;
        }
        // Press R to return to map select after game ends
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::R &&
            (gameState == STATE_GAME_OVER || gameState == STATE_WIN))
        {
            gameState = STATE_MAP_SELECT;
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            int mx = event.mouseButton.x;
            int my = event.mouseButton.y;

            if (gameState == STATE_MAP_SELECT) {
                handleMapSelectInput(mx, my);
            }
            else if (gameState == STATE_PLAYING) {
                // If the shop is already open, clicks on a tower select it
                if (showShopDetails) {
                    bool towerClicked = false;
                    for (int i = 0; i < towerCount; i++) {
                        if (!towers[i]->isAlive()) continue;
                        float dx = mx - towers[i]->getX();
                        float dy = my - towers[i]->getY();
                        if (dx * dx + dy * dy < 30 * 30) {   // click within 30px radius
                            selectedTower = towers[i];
                            shopSelectedIndex = -1;     // indicate upgrade mode
                            towerClicked = true;
                            break;
                        }
                    }
                    if (!towerClicked) {
                        // If clicked on empty space, deselect any tower
                        selectedTower = nullptr;
                        shopSelectedIndex = 0;
                    }
                    // Don't close the shop here; let the shop input handler do it
                }
                else {
                    // Shop closed: first check if we clicked on a tower
                    bool towerClicked = false;
                    for (int i = 0; i < towerCount; i++) {
                        if (!towers[i]->isAlive()) continue;
                        float dx = mx - towers[i]->getX();
                        float dy = my - towers[i]->getY();
                        if (dx * dx + dy * dy < 30 * 30) {
                            selectedTower = towers[i];
                            showShopDetails = true;
                            shopSelectedIndex = -1;    // upgrade mode
                            towerClicked = true;
                            break;
                        }
                    }
                    if (!towerClicked) {
                        // Otherwise, check buildable tile
                        int col = mx / TILE;
                        int row = my / TILE;
                        if (map->isBuildable(col, row)) {
                            selectedTower = nullptr;
                            selectedCol = col;
                            selectedRow = row;
                            showShopDetails = true;
                            shopSelectedIndex = 0;     // buy mode
                        }
                    }
                }
            }
            else {
                // click anywhere on game-over / win screen → map select
                gameState = STATE_MAP_SELECT;
            }
        }
    }
}

void Game::update(float dt) {
    if (gameState != STATE_PLAYING) return;

    // --- spawn enemies ---
    spawnTimer += dt;
    if (enemiesSpawned < enemiesToSpawn && spawnTimer >= spawnInterval) {
        spawnEnemy();
        spawnTimer = 0.f;
    }

    // --- enemy–tower engagement (must come before enemy update) ---
    updateEnemyAttacks();

    // --- update all enemies ---
    for (int i = 0; i < enemyCount; i++)
        enemies[i]->update(dt);

    // In Game::update(), after enemy attack block, before tower update loop:
    for (int i = 0; i < towerCount; i++) {
        towers[i]->setBuffs(1.0f, 1.0f);   // reset to normal
    }

 
    for (int i = 0; i < towerCount; i++) {
        if (!towers[i]) continue;             

        AbdTower* at = dynamic_cast<AbdTower*>(towers[i]);
        if (at) {
            for (int j = 0; j < towerCount; j++) {
                if (!towers[j]) continue;      
                if (towers[j] != towers[i])
                    at->applyBuffTo(towers[j]);
            }
        }
    }
       // --- update towers and collect tower bullets ---
    for (int i = 0; i < towerCount; i++) {
        if (!towers[i]) continue;
        towers[i]->update(dt);
        if (towers[i]->canFire()) {
            Bullet* b = towers[i]->attack(enemies, enemyCount);
            if (b && bulletCount < MAX_BULLETS) {
                bullets[bulletCount++] = b;

                // Play the correct firing sound based on tower type
                if (dynamic_cast<CannonTower*>(towers[i]))
                    cannonSound.play();
                else if (dynamic_cast<snipertower*>(towers[i]))
                    sniperSound.play();
                else if (dynamic_cast<machineguntower*>(towers[i]))
                    machineSound.play();
                // SlowTower and AbdTower don't fire bullets, so no sound here
            }
        }
    }

    // --- update bullets ---
    for (int i = 0; i < bulletCount; i++)
        bullets[i]->update(dt);

    checkBulletCollisions();
    cleanupBullets();
    cleanupDeadTowers();
    checkEnemiesReachedEnd();
    cleanupDead();

    // --- wave transition ---
    if (enemiesSpawned >= enemiesToSpawn && enemyCount == 0) {
        wave++;
        if (wave > 5) {
            gameState = STATE_WIN;
            calculateScore();
            winSound.play();
            return;
        }
        startWave(wave);
    }
}

// -----------------------------------------------------------------------
// Enemy attacks on towers
// -----------------------------------------------------------------------

void Game::updateEnemyAttacks() {
    for (int i = 0; i < enemyCount; i++) {
        enemies[i]->setAttacking(false);   // reset every frame

        for (int j = 0; j < towerCount; j++) {
            if (!towers[j]->isAlive()) continue;

            float dx = enemies[i]->getX() - towers[j]->getX();
            float dy = enemies[i]->getY() - towers[j]->getY();
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist <= enemies[i]->getAttackRange()) {
                enemies[i]->setAttacking(true);

                // Fire a bullet toward the tower if cooldown is ready
                if (enemies[i]->canAttack() && bulletCount < MAX_BULLETS) {
                    bullets[bulletCount++] = new Bullet(
                        enemies[i]->getX(), enemies[i]->getY(),
                        towers[j]->getX(), towers[j]->getY(),
                        enemies[i]->getAttackDamage(),
                        true,                            // this is an enemy bullet
                        sf::Color(255, 120, 0),          // orange
                        180.f                            // speed
                    );
                    enemies[i]->resetAttackCooldown();
                    enemyFireSound.play();
                }
                break;   // only engage one tower at a time
            }
        }
    }
}

// -----------------------------------------------------------------------
// Bullet collision
// -----------------------------------------------------------------------

void Game::checkBulletCollisions() {
    for (int i = 0; i < bulletCount; i++) {
        if (!bullets[i]->isActive()) continue;

        if (bullets[i]->isFromEnemy()) {
            // hits towers
            for (int j = 0; j < towerCount; j++) {
                if (!towers[j]->isAlive()) continue;
                float dx = bullets[i]->getX() - towers[j]->getX();
                float dy = bullets[i]->getY() - towers[j]->getY();
                if (std::sqrt(dx * dx + dy * dy) < 20.f) {
                    towers[j]->takeDamage(bullets[i]->getDamage());
                    bullets[i]->deactivate();
                    break;
                }
            }
        }
        else {
            // hits enemies
            for (int j = 0; j < enemyCount; j++) {
                if (!enemies[j]->isAlive()) continue;
                float dx = bullets[i]->getX() - enemies[j]->getX();
                float dy = bullets[i]->getY() - enemies[j]->getY();
                if (std::sqrt(dx * dx + dy * dy) < 15.f) {
                    enemies[j]->takeDamage(bullets[i]->getDamage());
                    bullets[i]->deactivate();
                    break;
                }
            }
        }
    }
}

// -----------------------------------------------------------------------
// Cleanup helpers
// -----------------------------------------------------------------------

void Game::cleanupDead() {
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i]->isAlive()) {
            gold += enemies[i]->getReward();
            delete enemies[i];
            for (int j = i; j < enemyCount - 1; j++) enemies[j] = enemies[j + 1];
            enemyCount--;
            i--;
        }
    }
}

void Game::cleanupDeadTowers() {
    for (int i = 0; i < towerCount; i++) {
        if (!towers[i]->isAlive()) {
            towerDestroySound.play();
            delete towers[i];
            for (int j = i; j < towerCount - 1; j++) towers[j] = towers[j + 1];
            towerCount--;
            i--;
        }
    }
}

void Game::cleanupBullets() {
    for (int i = 0; i < bulletCount; i++) {
        if (!bullets[i]->isActive()) {
            delete bullets[i];
            for (int j = i; j < bulletCount - 1; j++) bullets[j] = bullets[j + 1];
            bulletCount--;
            i--;
        }
    }
}

void Game::checkEnemiesReachedEnd() {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i]->atend()) {
            enemyLeaveSound.play();
            lives--;
            delete enemies[i];
            for (int j = i; j < enemyCount - 1; j++) enemies[j] = enemies[j + 1];
            enemyCount--;
            i--;
            if (lives <= 0) {
                lives = 0;
                gameState = STATE_GAME_OVER;
                calculateScore();
                loseSound.play();
            }
        }
    }
}

// -----------------------------------------------------------------------
// Spawn
// -----------------------------------------------------------------------

void Game::spawnEnemy() {
    if (enemyCount >= MAX_ENEMIES) return;
    sf::Vector2f* path = map->getPath();
    int            pLen = map->getPathLength();
    sf::Vector2f   start = path[0];

    Enemy* e = nullptr;
    switch (wave) {
    case 1: e = new BasicEnemy(start.x, start.y, path, pLen); break;
    case 3:  switch (rand() % 2) {
        case 0: e = new fastenemy(start.x, start.y, path, pLen); break;
        case 1: e = new flyingenemy(start.x, start.y, path, pLen); break;
        }
        break;
    case 2: e = (rand() % 2 == 0)
        ? (Enemy*)new BasicEnemy(start.x, start.y, path, pLen)
        : (Enemy*)new fastenemy(start.x, start.y, path, pLen); break;
    case 4: e = new tankenemy(start.x, start.y, path, pLen); break;
    case 5:
       
        switch (rand() % 3) {
        case 0: e = new fastenemy(start.x, start.y, path, pLen); break;
        case 1: e = new tankenemy(start.x, start.y, path, pLen); break;
        case 2: e = new ShieldedEnemy(start.x, start.y, path, pLen); break;
        }
        break;
    default: e = new BasicEnemy(start.x, start.y, path, pLen); break;
    }

    enemies[enemyCount++] = e;
    enemiesSpawned++;
}

void Game::startWave(int waveNum) {
    enemiesSpawned = 0;
    spawnTimer = 0.f;
    enemiesToSpawn = 5 + waveNum * 3;
    float interval = 1.5f - waveNum * 0.15f;
    spawnInterval = (interval < 0.5f) ? 0.5f : interval;
}

// -----------------------------------------------------------------------
// Render
// -----------------------------------------------------------------------

void Game::render() {
    if (gameState == STATE_MAP_SELECT) {
        renderMapSelect();
        return;
    }

    window.clear(sf::Color(30, 30, 30));
    map->render(window);

    for (int i = 0; i < towerCount; i++) {
        if (towers[i] == selectedTower) {
            sf::CircleShape sel(20.f);
            sel.setFillColor(sf::Color::Transparent);
            sel.setOutlineColor(sf::Color::Yellow);
            sel.setOutlineThickness(2.f);
            sel.setOrigin(20.f, 20.f);
            sel.setPosition(towers[i]->getX(), towers[i]->getY());
            window.draw(sel);
        }
        towers[i]->render(window);
    }
    for (int i = 0; i < enemyCount; i++) enemies[i]->render(window);
    for (int i = 0; i < bulletCount; i++) bullets[i]->render(window);

    drawHUD();

    if (gameState == STATE_GAME_OVER || gameState == STATE_WIN) {
        // Semi-transparent overlay
        sf::RectangleShape overlay(sf::Vector2f(1000.f, 600.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 140));
        window.draw(overlay);

        char buf[80];
        if (gameState == STATE_GAME_OVER) {
            sf::Text msg("GAME OVER", font, 70);
            msg.setFillColor(sf::Color(220, 50, 50));
            msg.setPosition(270.f, 180.f);
            window.draw(msg);
        }
        else {
            sf::Text msg("YOU WIN!", font, 70);
            msg.setFillColor(sf::Color(50, 220, 80));
            msg.setPosition(310.f, 180.f);
            window.draw(msg);
        }

        sprintf_s(buf, "Score: %d      Best: %d", score, highScore);
        sf::Text scoreTxt(buf, font, 28);
        scoreTxt.setFillColor(sf::Color::Yellow);
        scoreTxt.setPosition(300.f, 280.f);
        window.draw(scoreTxt);

        sf::Text hint("Press R or click to return to map select", font, 20);
        hint.setFillColor(sf::Color(200, 200, 200));
        hint.setPosition(230.f, 340.f);
        window.draw(hint);
    }




    if (showShopDetails && gameState == STATE_PLAYING) {
        // Semi-transparent background
        sf::RectangleShape bg(sf::Vector2f(300.f, 400.f));
        bg.setPosition(820.f, 50.f);
        bg.setFillColor(sf::Color(20, 20, 30, 230));
        bg.setOutlineColor(sf::Color(255, 255, 255, 80));
        bg.setOutlineThickness(2.f);
        window.draw(bg);

        if (selectedTower != nullptr) {
            // ----- UPGRADE MODE -----
            sf::Text title("Tower Info", font, 22);
            title.setFillColor(sf::Color::White);
            title.setPosition(840.f, 60.f);
            window.draw(title);

            // Tower type name (we can get via dynamic_cast, but simpler: use a string tag)
            // We'll determine the name by checking type.
            std::string towerName = "Tower";
            if (dynamic_cast<CannonTower*>(selectedTower)) towerName = "Cannon";
            else if (dynamic_cast<snipertower*>(selectedTower)) towerName = "Sniper";
            else if (dynamic_cast<machineguntower*>(selectedTower)) towerName = "Machine Gun";
            else if (dynamic_cast<slowtower*>(selectedTower)) towerName = "Slow Tower";
            else if (dynamic_cast<AbdTower*>(selectedTower)) towerName = "Buff Tower";

            char buf[100];
            sprintf_s(buf, "%s  Lv.%d", towerName.c_str(), selectedTower->getLevel());
            sf::Text nameTxt(buf, font, 18);
            nameTxt.setFillColor(sf::Color::Yellow);
            nameTxt.setPosition(840.f, 100.f);
            window.draw(nameTxt);

            sprintf_s(buf, "Dmg: %.0f   Range: %.0f", selectedTower->getEffectiveDamage(), selectedTower->getRange());
            sf::Text statsTxt(buf, font, 14);
            statsTxt.setFillColor(sf::Color::White);
            statsTxt.setPosition(840.f, 130.f);
            window.draw(statsTxt);

            if (selectedTower->canUpgrade()) {
                sprintf_s(buf, "Upgrade cost: %d gold", selectedTower->getUpgradeCost());
                sf::Text costTxt(buf, font, 14);
                costTxt.setFillColor(sf::Color(200, 200, 0));
                costTxt.setPosition(840.f, 160.f);
                window.draw(costTxt);

                // Upgrade button
                sf::RectangleShape upBtn(sf::Vector2f(200.f, 40.f));
                upBtn.setPosition(850.f, 200.f);
                upBtn.setFillColor(sf::Color(100, 200, 100));
                upBtn.setOutlineColor(sf::Color::White);
                upBtn.setOutlineThickness(1.f);
                window.draw(upBtn);

                sf::Text upText("UPGRADE", font, 20);
                upText.setFillColor(sf::Color::White);
                upText.setPosition(910.f, 207.f);
                window.draw(upText);
            }
            else {
                sf::Text maxText("MAX LEVEL", font, 14);
                maxText.setFillColor(sf::Color(255, 100, 100));
                maxText.setPosition(840.f, 160.f);
                window.draw(maxText);
            }

            sf::Text closeHint("Right-click to deselect", font, 12);
            closeHint.setFillColor(sf::Color(180, 180, 180));
            closeHint.setPosition(840.f, 260.f);
            window.draw(closeHint);

        }
        else {
            // ----- BUY MODE (original tower selection) -----
            sf::Text title("Choose Tower", font, 22);
            title.setFillColor(sf::Color::White);
            title.setPosition(840.f, 60.f);
            window.draw(title);

            for (int i = 0; i < 5; i++) {
                float yOff = 100.f + i * 58.f;
                sf::RectangleShape row(sf::Vector2f(280.f, 50.f));
                row.setPosition(830.f, yOff);
                if (i == shopSelectedIndex)
                    row.setFillColor(sf::Color(100, 200, 100, 100));
                else
                    row.setFillColor(sf::Color(50, 50, 50, 200));
                row.setOutlineColor(sf::Color(255, 255, 255, 40));
                row.setOutlineThickness(1.f);
                window.draw(row);

                // name & cost
                char line1[60];
                sprintf_s(line1, "%s (%dg)", towerInfos[i].name, towerInfos[i].cost);
                sf::Text t1(line1, font, 14);
                t1.setFillColor(sf::Color::White);
                t1.setPosition(838.f, yOff + 2.f);
                window.draw(t1);

                // stats
                char line2[80];
                if (towerInfos[i].damage > 0)
                    sprintf_s(line2, "R:%.0f  Dmg:%.0f  Spd:%.1f", towerInfos[i].range,
                        towerInfos[i].damage, towerInfos[i].fireRate);
                else
                    sprintf_s(line2, "R:%.0f  Slows / Buffs", towerInfos[i].range);
                sf::Text t2(line2, font, 12);
                t2.setFillColor(sf::Color(200, 200, 200));
                t2.setPosition(838.f, yOff + 20.f);
                window.draw(t2);

                // description
                sf::Text t3(towerInfos[i].desc, font, 11);
                t3.setFillColor(sf::Color(180, 180, 180));
                t3.setPosition(838.f, yOff + 35.f);
                window.draw(t3);
            }

            sf::Text buyHint("Click row to buy, right-click to cancel", font, 13);
            buyHint.setFillColor(sf::Color(255, 255, 100));
            buyHint.setPosition(830.f, 400.f);
            window.draw(buyHint);
        }
    }

    window.display();
}

// -----------------------------------------------------------------------
// Map selection screen
// -----------------------------------------------------------------------

void Game::renderMapSelect() {
    window.clear(sf::Color(15, 15, 35));

    sf::Text title("SELECT A MAP", font, 52);
    title.setFillColor(sf::Color::White);
    title.setPosition(280.f, 40.f);
    window.draw(title);

    const char* mapNames[3] = {
        "Map 1 :  Classic Winding",
        "Map 2 :  Long S-Curve",
        "Map 3 :  Zigzag Rush"
    };
    sf::Color btnColors[3] = {
        sf::Color(60, 100, 60),
        sf::Color(80,  60, 100),
        sf::Color(100, 70,  40)
    };

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape btn(sf::Vector2f(400.f, 90.f));
        btn.setPosition(300.f, 140.f + i * 130.f);
        btn.setFillColor(btnColors[i]);
        btn.setOutlineColor(sf::Color::White);
        btn.setOutlineThickness(2.f);
        window.draw(btn);

        sf::Text nameTxt(mapNames[i], font, 26);
        nameTxt.setFillColor(sf::Color::White);
        nameTxt.setPosition(316.f, 155.f + i * 130.f);
        window.draw(nameTxt);

        char buf[40];
        sprintf_s(buf, "Best score: %d", mapHighScores[i]);
        sf::Text hsTxt(buf, font, 18);
        hsTxt.setFillColor(sf::Color(220, 220, 80));
        hsTxt.setPosition(316.f, 192.f + i * 130.f);
        window.draw(hsTxt);
    }

    window.display();
}

void Game::handleMapSelectInput(int mx, int my) {
    for (int i = 0; i < 3; i++) {
        sf::IntRect btn(300, 140 + i * 130, 400, 90);
        if (btn.contains(mx, my)) {
            resetGame(i + 1);
            return;
        }
    }
}




// -----------------------------------------------------------------------
// HUD
// -----------------------------------------------------------------------

void Game::drawHUD() {
    char buf[120];
    sprintf_s(buf, "Wave: %d/5   Gold: %d   Lives: %d   Map: %d   Best: %d",
        wave, gold, lives, selectedMapIndex, highScore);
    hudText.setString(buf);
    window.draw(hudText);
}

// -----------------------------------------------------------------------
// High score file I/O
// -----------------------------------------------------------------------

void Game::loadAllHighScores() {
    for (int i = 0; i < 3; i++)
        mapHighScores[i] = loadHighScore(i + 1);
}

int Game::loadHighScore(int mapIdx) {
    char filename[40];
    sprintf_s(filename, "highscore_map%d.txt", mapIdx);
    std::ifstream f(filename);
    int hs = 0;
    if (f.is_open()) { f >> hs; f.close(); }
    return hs;
}

void Game::saveHighScore(int mapIdx, int hs) {
    char filename[40];
    sprintf_s(filename, "highscore_map%d.txt", mapIdx);
    std::ofstream f(filename);
    if (f.is_open()) { f << hs; f.close(); }
}