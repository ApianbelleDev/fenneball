#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"


#include "obj.h"
#include "defines.h"

struct Paddle playerPaddle;
struct Paddle enemyPaddle;
struct Ball ball;

int logoTimer;
int gameTimer;
int playerScore = 0;
int enemyScore = 0;
int cur = 0;
int curX, curY;

int enemyTargetY;
bool isTwoPlayer = false;

//define textures for loading
Texture2D paddleTex;
Texture2D ballTex; 	
Texture2D titleTex;
Texture2D curTex;
Image icon;

int mod (int a, int b) {
   if(b < 0) //you can check for b == 0 separately and do what you want
     return -mod(-a, -b);   
   int ret = a % b;
   if(ret < 0)
     ret+=b;
   return ret;
}

int randRange(int l, int u){
	return (rand() % (u - l + 1)) + l;
}

void initOBJ(){
	//init structs and enums
	playerPaddle.sx = 0;
	playerPaddle.sy = 100;
	playerPaddle.x = playerPaddle.sx;
	playerPaddle.y = playerPaddle.sy;
	playerPaddle.vel = 0;
	playerPaddle.accel = 1;
	playerPaddle.maxSpeed = 3;
	playerPaddle.speed = 3;
	playerPaddle.w = 16;
	playerPaddle.h = 64;

	enemyPaddle.sx = 320 - 16;
	enemyPaddle.sy = 100;
	enemyPaddle.x = enemyPaddle.sx;
	enemyPaddle.y = enemyPaddle.sy;
	enemyPaddle.speed = 2;
	enemyPaddle.w = 16;
	enemyPaddle.h = 64;
	enemyPaddle.target = ball.x + 1;

	ball.x = SCREEN_XRES / 2;
	ball.y = SCREEN_YRES / 2;
	ball.dx = -2;
	ball.dy = 2;
	ball.w = 8;
	ball.h = 8;

	//load images into texture variables
	paddleTex   = LoadTexture("res/paddle.png");
	ballTex     = LoadTexture("res/ball.png");
	titleTex    = LoadTexture("res/title.png");
	curTex      = LoadTexture("res/cur.png");

	icon        = LoadImage("res/ico.png");

	//init cursor position
	curX = 100;
	curY = 150;
}

void PredictBallY(){
	//get the time it takes for the ball to hit the right side of the screen
	int remainingTime = (SCREEN_XRES - ball.x) / ball.dx;

	//get the projected ball Y pos
	int projectedYPos = ball.y + ball.dy * remainingTime;

	projectedYPos = mod(projectedYPos, 2 * SCREEN_YRES);
	if (projectedYPos > SCREEN_YRES) {
		projectedYPos = 2 * SCREEN_YRES - projectedYPos;
	}

	enemyTargetY = projectedYPos + randRange(10 + enemyPaddle.w / 2, 10 - -enemyPaddle.w / 2);
}

void updateOBJ(){

	//UPDATE BALL

	//at the beginning of each gameplay round, increment gameTimer. when it reaches 60(1 second), start moving the ball until it goes offscreen and objects(including gameTimer) reset
	if(gameTimer > 0){
		gameTimer--;
	}
	if(gameTimer <= 0){
		ball.x += ball.dx;
		ball.y += ball.dy;

		if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
			playerPaddle.vel -= playerPaddle.accel;
		}
		else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
			playerPaddle.vel += playerPaddle.accel;
		}

		else if(playerPaddle.vel > 0) {
			playerPaddle.vel -= playerPaddle.accel;
			if(playerPaddle.vel < 0) {
				playerPaddle.vel = 0;
			}
		}
		else if(playerPaddle.vel < 0) {
			playerPaddle.vel+= playerPaddle.accel;
			if(playerPaddle.vel > 0) {
				playerPaddle.vel = 0;
			}
		}

		playerPaddle.y += playerPaddle.vel;

		if (playerPaddle.vel > playerPaddle.maxSpeed){
			playerPaddle.vel = playerPaddle.maxSpeed;
		}
		else if(playerPaddle.vel < -playerPaddle.maxSpeed){
			playerPaddle.vel = -playerPaddle.maxSpeed;
		}

		if (playerPaddle.y <= 0) {
			playerPaddle.y = 0;
			playerPaddle.vel = 0;
		}
		else if (playerPaddle.y >= 170) {
			playerPaddle.y = 170;
			playerPaddle.vel = 0;
		}



		if(!isTwoPlayer){
			//ENEMY AI
			if (enemyPaddle.y + enemyPaddle.h / 2 > enemyTargetY) {
				enemyPaddle.y -= enemyPaddle.speed;
			}
			if (enemyPaddle.y + enemyPaddle.h / 2 < enemyTargetY) {
				enemyPaddle.y += enemyPaddle.speed;
			}
		}	else {
			if(IsKeyDown(KEY_DOWN)){
				enemyPaddle.y += playerPaddle.speed;
			}
			else if(IsKeyDown(KEY_UP)){
				enemyPaddle.y -= playerPaddle.speed;
			}
		}
		
		if (enemyPaddle.y <= 0){
			enemyPaddle.y = 0;
		}
		else if (enemyPaddle.y >= 170){
		enemyPaddle.y = 170;
		}
		//COLLISION
		// ball to bottom screen bound
		if (ball.y >= 220){
			ball.dy = -2;
		}
		// ball to top screen bound
		else if(ball.y <= 0){
			ball.dy = 2;
		}
	}
		//ball to left paddle
		if (ball.x <= playerPaddle.x + playerPaddle.w && ball.y <= playerPaddle.y + playerPaddle.h && ball.y >= playerPaddle.y) { 
			ball.dx = 2;
			PredictBallY();
		}
		//ball to right paddle
		else if (ball.x >= enemyPaddle.x - enemyPaddle.w / 2 && ball.y <= enemyPaddle.y + enemyPaddle.h && ball.y >= enemyPaddle.y) { 
			ball.dx = -2;
		}

}

void resetOBJ(){
	//reset ballpos and both paddlepos when ball goes out of play area
	if (ball.x <= 0 || ball.x >= 340) {
		if(ball.x <= 0){
			enemyScore++;
			printf("%i - %i\n", playerScore, enemyScore);
		}
		else if(ball.x >=340){
			playerScore++;
			printf("%i - %i\n", playerScore, enemyScore);
		}
		ball.x = SCREEN_XRES / 2;
		ball.y = SCREEN_YRES / 2;
		ball.dx = -2;
		ball.dy = 2;
		playerPaddle.x = playerPaddle.sx;
		playerPaddle.y = playerPaddle.sy;
		enemyPaddle.x = enemyPaddle.sx;
		enemyPaddle.y = enemyPaddle.sy;
		gameTimer = 60;
		playerPaddle.vel = 0;
	}
}


int main(void) {
	enum GameStates curState = GAMEPLAY;
	logoTimer = 180;
	gameTimer = 60;

	//initialize window
	InitWindow(SCREEN_XRES, SCREEN_YRES, "Fenneball");

	//hide cursor so it doesn't interfere with window
	HideCursor();

	SetTargetFPS(60);

	initOBJ();

	while(!WindowShouldClose()) {
		//set icons
		SetWindowIcon(icon);
		
		switch(curState){
			case LOGO:{
				//logo code here(3 second timer showing a fullscreen image with possibly a fennekin or something idk.)
				logoTimer--;
				if (logoTimer <= 0){
					curState = TITLE;
				}

			} break;
			case TITLE:{
			//title code here
			if(IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)){
				cur++;
				printf("%i\n", cur);
			}
			else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
				cur--;
				printf("%i\n", cur);

			}
			if(cur == 0){
				if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
					curState = GAMEPLAY;
				}
				curY = 155;
				
			}
		  if(cur == 1){
					if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
						isTwoPlayer = true;
				 		curState = GAMEPLAY;
					}
				curY = 175;
			}
			if(cur == 2){
				if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
					CloseWindow();
				}
				curY = 195;
			}

			//cursor wrap around
			cur = cur % 3;
			} break;
			case GAMEPLAY:{
				resetOBJ();
				updateOBJ();
			} break;
			default: break;
		}
		BeginDrawing();
			if(curState == TITLE){
				DrawTexture(titleTex, 0, 0, WHITE);
				DrawTexture(curTex, curX, curY, WHITE);

			}
			else if(curState == GAMEPLAY){
				ClearBackground(BLACK);
				DrawTexture(paddleTex, playerPaddle.x, playerPaddle.y, WHITE);
				DrawTexture(paddleTex, enemyPaddle.x, enemyPaddle.y, WHITE);
				DrawTexture(ballTex, ball.x, ball.y, WHITE);
				DrawText(TextFormat("%i - %i", playerScore, enemyScore), SCREEN_XRES / 2 - 20, 30, 20, WHITE);
				//DrawRectangle(SCREEN_XRES - 16, enemyTargetY, 16, 16, RED);
			}
			//draw here
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
