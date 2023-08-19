#pragma once
#ifndef OBJ_H

#include <stdbool.h>

struct Paddle {
	//sx = start x, sy = start y
	double sx, sy, y, x;
	double speed;
	int w, h;
};

struct Ball {
	double x, y;
	double dx, dy;
	int w, h;
	bool isMoving;
};

struct Game {
	bool isRunning;
};

typedef enum GameStates {
 LOGO,
 TITLE,
 GAMEPLAY
};

#endif