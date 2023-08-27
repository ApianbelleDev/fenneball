#pragma once
#ifndef OBJ_H

#include "raylib.h"

struct Paddle {
	//sx = start x, sy = start y
	float sx, sy, y, x;
	float vel, accel, maxSpeed;
	float speed;
	int w, h;
	int target;
};

struct Ball {
	float x, y;
	float dx, dy;
	int w, h;
};

typedef enum GameStates {
 LOGO,
 TITLE,
 GAMEPLAY
};

#endif