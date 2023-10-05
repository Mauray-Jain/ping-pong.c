#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "lib/ball/ball.h"
#include "lib/paddle/paddle.h"

#define FPS 30

void must_init(int cond, char* msg);

int main(int argc, char *argv[]){
	Display* dsp = XOpenDisplay(NULL);
	must_init(!dsp, "Couln't open display");

	int root =  DefaultRootWindow(dsp);
	int defaultScreen = DefaultScreen(dsp);
	int width = 800;
	int height = 800;
	int screenBitDepth = 24;

	// Visual Info
	XVisualInfo visInfo = {};
	must_init(!XMatchVisualInfo(dsp, defaultScreen, screenBitDepth, TrueColor, &visInfo), "No matching avaiable");

	// Setting window attributes
	XSetWindowAttributes winAttr;
	// winAttr.bit_gravity = StaticGravity;
	winAttr.background_pixel = 0;
	winAttr.colormap = XCreateColormap(dsp, root, visInfo.visual, AllocNone);
	winAttr.event_mask = StructureNotifyMask | KeyPressMask;
	unsigned long attrMask = CWColormap | /* CWBitGravity | */ CWBackPixel | CWEventMask;

	// Making the window
	Window window = XCreateWindow(dsp, root, 0, 0, width, height, 0, visInfo.depth, InputOutput, visInfo.visual, attrMask, &winAttr);
	must_init(!window, "No window");
	XStoreName(dsp, window, "Pong");
	XMapWindow(dsp, window);
	XFlush(dsp);

	// Making a GC
	XGCValues gcValues;
	GC gc = XCreateGC(dsp, window, 0, &gcValues);
	XSetForeground(dsp, gc, WhitePixel(dsp, defaultScreen));
	XSetBackground(dsp, gc, BlackPixel(dsp, defaultScreen));
	XSetLineAttributes(dsp, gc, 20, LineSolid, CapRound, JoinRound);
	XSetFillStyle(dsp, gc, FillSolid);
	XSync(dsp, False);

	// No error on shutdown
	Atom WM_DELETE_WINDOW = XInternAtom(dsp, "WM_DELETE_WINDOW", False);
	if(!XSetWMProtocols(dsp, window, &WM_DELETE_WINDOW, 1)){
		perror("No graceful shutdow");
	}

	// Generate a ball
	struct pong_ball ball;
	pong_ball_gen(&ball, width / 2, height / 4, 0);

	// Generate a paddle
	struct pong_paddle paddle;
	pong_paddle_gen(&paddle, width / 2, height - 45, 15, width / 4);

	unsigned int score = 0;
	char score_buf[11];
	XFontStruct* font = XLoadQueryFont(dsp, "fixed");
	XCharStruct overall;
	int direction, ascent, descent;
	XTextExtents(font, score_buf, strlen(score_buf), &direction, &ascent, &descent, &overall);
	XSetFont(dsp, gc, font->fid);

	int winOpen = 1;
	while(winOpen){

		if(ball.x >= width - ball.radius || ball.x < 0)
			pong_ball_reflect(&ball, 'x');
		if(ball.y >= height - ball.radius || ball.y < 0)
			pong_ball_reflect(&ball, 'y');
		if(ball.y > paddle.y - 10 - ball.radius && (ball.x > paddle.x && ball.x < paddle.x + paddle.width)){
			// Only reflect balls coming from up
			if(ball.vy > 0){
				ball.vy *= -1;
				snprintf(score_buf, sizeof score_buf, "%u", ++score);
			}
			if((score + 1) % 50 == 0){
				ball.vx++;
				ball.vy++;
			}
		}
		// if(ball.y >= height - ball.radius)
		// if(ball.y <= paddle.y - 10 && (ball.x > paddle.x && ball.x < paddle.x + paddle.width)) // For cheat mode
		XFillArc(dsp, window, gc, ball.x, ball.y, ball.radius, ball.radius, 0, 360*64);
		XDrawRectangle(dsp, window, gc, paddle.x, paddle.y, paddle.width, 0);
		XDrawImageString(dsp, window, gc, width / 2, 45, score_buf, strlen(score_buf));
		XFlush(dsp);
		usleep(1000000 / FPS);
		XClearWindow(dsp, window);
		ball.x += ball.vx;
		ball.y += ball.vy;

		// Event loop
		XEvent ev;
		while(XPending(dsp) > 0){
			XNextEvent(dsp, &ev);
			switch(ev.type){
				case KeyPress: {
					XKeyPressedEvent* e = (XKeyPressedEvent*) &ev;
					if(e->keycode == XKeysymToKeycode(dsp, XK_Left) && paddle.x > 0)
						pong_paddle_move(&paddle, 'l');
					else if(e->keycode == XKeysymToKeycode(dsp, XK_Right) && (paddle.x + paddle.width) < width)
						pong_paddle_move(&paddle, 'r');
					else if(e->keycode == XKeysymToKeycode(dsp, XK_C))
						printf("Cheat mode on!\n");
				} break;
				case DestroyNotify: {
					XDestroyWindowEvent* e = (XDestroyWindowEvent*) &ev;
					winOpen = 0;
				} break;
				case ClientMessage: {
					XClientMessageEvent* e = (XClientMessageEvent*) &ev;
					if((Atom) e->data.l[0] == WM_DELETE_WINDOW){
						XFreeGC(dsp, gc);
						XDestroyWindow(dsp, window);
						winOpen = 0;
					}
				} break;
				case ConfigureNotify: {
					XConfigureEvent* e = (XConfigureEvent*) &ev;
					int initial_width = width;
					width = e->width;
					height = e->height;
					paddle.width = width / 4;
					// paddle.x = paddle.x * (width / initial_width);
					paddle.y = height - 45;
				} break;
			}
		}
	}

	XCloseDisplay(dsp);
	return 0;
}

void must_init(int cond, char* msg){
	if(cond){
		perror(msg);
		exit(EXIT_FAILURE);
	}
}
