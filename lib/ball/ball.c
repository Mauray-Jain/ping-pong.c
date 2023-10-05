#include "ball.h"
#include <stdlib.h>
#include <time.h>

void pong_ball_gen(struct pong_ball* ball, int x, int y, int radius){
	srand(time(NULL));
	int arr[] = {-1, 1};
	ball->x = x;
	ball->y = y;
	int positivevx = arr[rand() % 2];
	int positivevy = arr[rand() % 2];
	ball->vx = positivevx * ((rand() % 10) + 10);
	ball->vy = positivevy * ((rand() % 10) + 10);
	ball->radius = ((radius != 0) ? radius : 50);
}

void pong_ball_reflect(struct pong_ball* ball, char about){
	if(about == 'x')
		ball->vx *= -1;
	else if(about == 'y')
		ball->vy *= -1;
}
