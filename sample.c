#include <stdio.h>
#include <stdlib.h>

#include "X11Macros.h"		// X11 macros

#define X_RESN	800       	// x resolution
#define Y_RESN	800       	// y resolution


int main (int argc, char **argv ) {
    int x,y;
/* --------------------------- X11 graphics setup ------------------------------ */

	initX11(X_RESN,Y_RESN);

/* ----- End of X11 graphics setup, continue with application code, sample given here ----- */

     for (x = 0; x < 800; x = x + 1) {
	y = x;

	XClearWindow(display, win);  				// clear window for next drawing
	XSetForeground(display,gc,(long) 0xDC143C);  		// color of foreground (applies to object to be drawn)

	//XDrawPoint (display, win, gc, x, y); 			// draw point at location x, y in window

	XFillArc (display,win,gc,x-25,y-25,50,50,0,23040);	// draw circle of size 50x50 at location (x,y)
								// X11 parameters specify upper-left corner of bounding rectangle.

	XFlush(display);					// necessary to write to display

	usleep(100000);						// provide a delay beween each drawing

     }

	return 0;
}
