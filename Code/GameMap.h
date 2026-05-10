#pragma once
#ifndef GameMap_H
#define GameMap_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

const int TILE = 50;
const int COLS = 20;
const int ROWS = 12;
const int MAX_PATH_LEN = 60;   // enough for any of the 3 maps

class GameMap {
private:
    int mapIndex;
    int grid[ROWS][COLS];
    sf::Vector2f pathWaypoints[MAX_PATH_LEN];
    int waypointCount;

public:
    explicit GameMap(int idx = 1) : mapIndex(idx), waypointCount(0) {
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                grid[r][c] = 0;

        if (idx == 1) initMap1();
        else if (idx == 2) initMap2();
        else               initMap3();
    }

    void render(sf::RenderWindow& window) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                sf::RectangleShape tile(sf::Vector2f(TILE - 1, TILE - 1));
                tile.setPosition((float)(c * TILE), (float)(r * TILE));
                switch (grid[r][c]) {
                case 1:  tile.setFillColor(sf::Color(180, 140, 80)); break;  // path
                case 2:  tile.setFillColor(sf::Color(100, 80, 60)); break;  // tower
                default: tile.setFillColor(sf::Color(60, 120, 60)); break;  // grass
                }
                window.draw(tile);
            }
        }
    }

    bool isBuildable(int col, int row) const {
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return false;
        return grid[row][col] == 0;
    }

    void setBlocked(int col, int row) { grid[row][col] = 2; }
    sf::Vector2f* getPath() { return pathWaypoints; }
    int  getPathLength()         const { return waypointCount; }
    int  getMapIndex()           const { return mapIndex; }

private:
    // Helper: mark cells as path and fill waypoint array
    void buildPath(int cells[][2], int count) {
        waypointCount = count;
        for (int i = 0; i < count; i++) {
            int r = cells[i][0];
            int c = cells[i][1];
            grid[r][c] = 1;
            pathWaypoints[i] = sf::Vector2f(
                (float)(c * TILE + TILE / 2),
                (float)(r * TILE + TILE / 2));
        }
    }

    // ---------------------------------------------------------------
    // Map 1 – original winding path (enters left-middle, exits right)
    // ---------------------------------------------------------------
    void initMap1() {
        int cells[31][2] = {
            {5,0},{5,1},{5,2},{5,3},{5,4},{5,5},
            {4,5},{3,5},{2,5},{1,5},
            {1,6},{1,7},{1,8},{1,9},{1,10},
            {2,10},{3,10},{4,10},{5,10},{6,10},
            {7,10},{8,10},{8,11},{8,12},{8,13},
            {8,14},{8,15},{8,16},{8,17},{8,18},{8,19}
        };
        buildPath(cells, 31);
    }

    // ---------------------------------------------------------------
    // Map 2 – long S-curve (enters top-left, exits right side row 3)
    // ---------------------------------------------------------------
    void initMap2() {
        int cells[47][2] = {
            // top-left heading right
            {0,0},{0,1},{0,2},{0,3},{0,4},{0,5},
            // drop down col 5
            {1,5},{2,5},{3,5},{4,5},{5,5},{6,5},
            // row 6 heading left
            {6,4},{6,3},{6,2},{6,1},
            // drop down col 1 to bottom
            {7,1},{8,1},{9,1},{10,1},{11,1},
            // row 11 heading right
            {11,2},{11,3},{11,4},{11,5},{11,6},{11,7},{11,8},{11,9},{11,10},
            // col 10 heading up
            {10,10},{9,10},{8,10},{7,10},{6,10},{5,10},{4,10},{3,10},
            // row 3 heading right to exit
            {3,11},{3,12},{3,13},{3,14},{3,15},{3,16},{3,17},{3,18},{3,19}
        };
        buildPath(cells, 47);
    }

    // ---------------------------------------------------------------
    // Map 3 – zigzag (enters bottom-left, exits right side row 9)
    // ---------------------------------------------------------------
    void initMap3() {
        int cells[36][2] = {
            // enter bottom-left, head up col 0
            {11,0},{10,0},{9,0},{8,0},
            // row 8 heading right
            {8,1},{8,2},{8,3},{8,4},{8,5},{8,6},{8,7},{8,8},
            // col 8 heading up
            {7,8},{6,8},{5,8},{4,8},{3,8},{2,8},
            // row 2 heading right
            {2,9},{2,10},{2,11},{2,12},
            // col 12 heading down
            {3,12},{4,12},{5,12},{6,12},{7,12},{8,12},{9,12},
            // row 9 heading right to exit
            {9,13},{9,14},{9,15},{9,16},{9,17},{9,18},{9,19}
        };
        buildPath(cells, 36);
    }
};

#endif