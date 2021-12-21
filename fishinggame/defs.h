/*
 * Fishing Simulator 2021
 * CPE 353, Fall 2021
 * Made by:
 * Ben Bruzewski, Ben McAnulty, Jacob Smith, and Jesse Cowart
*/

#ifndef DEFS_H
#define DEFS_H

const int SCENE_WIDTH = 1391;        // Scene dimensions
const int SCENE_HEIGHT = 731;

const int PLAYER_WIDTH = 132;        // Player dimensions
const int PLAYER_HEIGHT = 132;

const int FRAME_RATE = 24;

const int OBSTACLE_WIDTH = 30;      // Obstacle dimensions
const int OBSTACLE_HEIGHT = 30;

const int FISH_WIDTH = 35;          // Fish dimensions
const int FISH_HEIGHT = 35;

const int indexes = 8; // these indexes specify which page the client will be on
const int start = 0;
const int join = 1;
const int loading = 2;
const int gameplay = 3;
const int scoreboard = 4;
const int quit = 5;
const int help = 6;
const int quitConfirm =7;

static int currentind;
static int quitint = 0;

#endif // DEFS_H
