#pragma once

struct pong_ball {
	int x;
	int y;
	int vx;
	int vy;
	int radius;
};

void pong_ball_gen(struct pong_ball*, int x, int y, int radius);
void pong_ball_reflect(struct pong_ball*, char about);
