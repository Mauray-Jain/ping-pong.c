#pragma once

struct pong_paddle {
	int x;
	int y;
	int vx;
	int width;
};

void pong_paddle_gen(struct pong_paddle*, int x, int y, int vx, int width);
void pong_paddle_move(struct pong_paddle*, char about);
