#include "paddle.h"

void pong_paddle_gen(struct pong_paddle* pad, int x, int y, int vx, int width){
	pad->x = x;
	pad->y = y;
	pad->vx = vx;
	pad->width = width;
}

void pong_paddle_move(struct pong_paddle* pad, char about){
	if(about == 'l')
		pad->x -= pad->vx;
	else if(about == 'r')
		pad->x += pad->vx;
}
