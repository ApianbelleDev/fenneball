#include <psxpad.h>

#include "render.h"
#include "obj.h"
#include "defines.h"

struct Paddle playerPaddle;
struct Paddle enemyPaddle;
struct Ball ball;

void initOBJ(){

	playerPaddle.sx = 0;
	playerPaddle.sy = 100;
	playerPaddle.x = playerPaddle.sx;
	playerPaddle.y = playerPaddle.sy;
	playerPaddle.speed = 2;
	playerPaddle.w = 16;
	playerPaddle.h = 64;

	enemyPaddle.sx = 320 - 16;
	enemyPaddle.sy = 100;
	enemyPaddle.x = enemyPaddle.sx;
	enemyPaddle.y = enemyPaddle.sy;
	enemyPaddle.speed = 2;
	enemyPaddle.w = 16;
	enemyPaddle.h = 64;

	ball.x = SCREEN_XRES / 2;
	ball.y = SCREEN_YRES / 2;
	ball.dx = 2;
	ball.dy = 2;
	ball.w = 8;
	ball.h = 8;
}

void updateOBJ(){
	PADTYPE *pad;

	pad = (PADTYPE*)padbuff[0];

	//constantly set the balls dx and dy values so it's always moving.
		ball.x -= ball.dx;
		ball.y += ball.dy;

		if (pad->stat == 0) {
			if ((pad->type == DIGITAL_PAD) || (pad->type == ANALOG_PAD) || (pad->type == DUALSHOCK)) {
				if (!(pad->btn&PAD_UP)) {
					if (playerPaddle.y <= 9){
						playerPaddle.speed = 0;
					}
					playerPaddle.y -= playerPaddle.speed;
				}
				else if(!(pad->btn&PAD_DOWN)) {
					if(playerPaddle.y >= 170){
						playerPaddle.speed = 0;
					}
					playerPaddle.y += playerPaddle.speed;
				}
				if (!(pad->btn&PAD_TRIANGLE)) {
					if(enemyPaddle.y <= 9){
						enemyPaddle.speed = 0;
					}
					enemyPaddle.y -= enemyPaddle.speed;
				}
				else if (!(pad->btn&PAD_CROSS)) {
					if(enemyPaddle.y >= 170){
						enemyPaddle.speed = 0;
					}
					enemyPaddle.y += enemyPaddle.speed;
				} else {
					playerPaddle.speed = 2;
					enemyPaddle.speed = 2;
				}
			}
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

		//ball to left paddle
		if (ball.x <= playerPaddle.x + playerPaddle.w && ball.y <= playerPaddle.y + playerPaddle.h && ball.y >= playerPaddle.y) { 
			ball.dx = -2;
		}
		//ball to right paddle
		else if (ball.x >= enemyPaddle.x - enemyPaddle.w / 2 && ball.y <= enemyPaddle.y + enemyPaddle.h && ball.y >= enemyPaddle.y) { 
			ball.dx = 2;
		}
}

void resetOBJ(){
	//reset ballpos and both paddlepos when ball goes out of play area
	if (ball.x <= 0 || ball.x >= 340) {
		ball.x = SCREEN_XRES / 2;
		ball.y = SCREEN_YRES / 2;
		playerPaddle.x = playerPaddle.sx;
		playerPaddle.y = playerPaddle.sy;
		enemyPaddle.x = enemyPaddle.sx;
		enemyPaddle.y = enemyPaddle.sy;
	}
}


int main(int argc, const char **argv) {
	
	// Initialize the GPU and load the default font texture provided by
	// PSn00bSDK at (960, 0) in VRAM.
	ResetGraph(0);
	FntLoad(960, 0);

	// Set up our rendering context.
	RenderContext ctx;
	setup_context(&ctx, SCREEN_XRES, SCREEN_YRES, 0, 0, 0);

	initOBJ();

	// initialize gamepads
	PADTYPE *pad;
	InitPAD(padbuff[0], 34, padbuff[1], 34);

	// begin polling
	StartPAD();

	// to avoid VSync Timeout error
	ChangeClearPAD(1);

	while(1) {
		resetOBJ();
		updateOBJ();

		// Draw the square by allocating a TILE (i.e. untextured solid color
		// rectangle) primitive at Z = 1.
		TILE *pPaddle  = (TILE *) new_primitive(&ctx, 1, sizeof(TILE));
		TILE *ePaddle  = (TILE *) new_primitive(&ctx, 1, sizeof(TILE));
		TILE *ballTile = (TILE *) new_primitive(&ctx, 1, sizeof(TILE));

		setTile(pPaddle);
		setXY0 (pPaddle, playerPaddle.x, playerPaddle.y);
		setWH  (pPaddle, playerPaddle.w, playerPaddle.h);
		setRGB0(pPaddle, 255, 255, 255);

		setTile(ePaddle);
		setXY0 (ePaddle, enemyPaddle.x, enemyPaddle.y);
		setWH  (ePaddle, enemyPaddle.w, enemyPaddle.h);
		setRGB0(ePaddle, 255, 255, 255);


		setTile(ballTile);
		setXY0 (ballTile, ball.x, ball.y);
		setWH  (ballTile, ball.w, ball.h);
		setRGB0(ballTile, 255, 255, 255);

		// Draw some text in front of the square (Z = 0, primitives with higher
		// Z indices are drawn first).
		draw_text(&ctx, 8, 16, 0, "controls:\n\ndpad up/down: move right paddle\nTriangle/Cross: move left paddle\n");

		flip_buffers(&ctx);
	}

	return 0;
}
