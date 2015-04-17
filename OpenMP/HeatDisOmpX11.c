#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>		// X11 library headers
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define X_RESN	800       	// x resolution
#define Y_RESN	800 
#define CHUNKSIZE 512
#define N 100
int main (int argc, char *argv[]) {
	Window	  win;			// initialization for a window
	unsigned int   width, height,	// window size */
                 win_x,win_y,		// window position
                 border_width,		// border width in pixels
                 display_width, display_height,  	// size of screen
                 screen;				// which screen

	char		*window_name = "My graphics program", *display_name = NULL;
	GC		gc;
	unsigned long	valuemask = 0;
	XGCValues	values;
	Display	  *display;
	Pixmap	  bitmap;
	XPoint	  points[800];
	FILE	  *fp, *fopen ();
	char	  str[100];
	
	XSetWindowAttributes attr[1];
       
	if (  (display = XOpenDisplay (display_name)) == NULL ) {   // connect to Xserver
	   fprintf (stderr, "Cannot connect to X server %s\n",XDisplayName (display_name) );
	   exit (-1);
	}

	screen = DefaultScreen (display);			// get screen size, not used here
	display_width = DisplayWidth (display, screen);
	display_height = DisplayHeight (display, screen);

	width = X_RESN;						// set window size
	height = Y_RESN;
	win_x = 0;	win_y = 0;					// set window position

	border_width = 4;						// create opaque window
	win = XCreateSimpleWindow (display, RootWindow (display, screen),
				win_x, win_y, width, height, border_width, 
				BlackPixel (display, screen), WhitePixel (display, screen));

	XStoreName(display, win, window_name);

	gc = XCreateGC (display, win, valuemask, &values);		// create graphics context

	XSetBackground (display, gc, WhitePixel (display, screen));
	XSetForeground (display, gc, BlackPixel (display, screen));
	XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

	XMapWindow (display, win);
	XSync(display, 0);

	int current, next, iteration, MAX_ITERATIONS, i, j, k, temp, x, y, count;

	double start, end, sTime, pTime;			// used for timing
	
	printf("Please input an integer value for the amount of iterations: ");
   	scanf("%i",&MAX_ITERATIONS);
	
	float h[2][N][N];
	
	printf("\nSequential, not parallelized\n");
	for (x = 0; x < 2; x++) {
		for (j = 0; j < N; j++)
			for (k = 0; k < N; k++){
			h[x][j][k] = 20;
			if((x == 0 && j == 0 && k >= (N/10)*4 && k <= (N/10)*5)){
			h[x][j][k] = 100.0;
			}}
				
	}
	for(i=0; i < N; i += (N/10))
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",h[0][i][j]);
     	}
	printf("\n");
	
	current = 0;
	next = 1;
	start = omp_get_wtime();
	for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
		for (i = 1; i < N-1; i++){
			for (j = 1; j < N-1; j++){
			h[next][i][j] = 0.25 * (h[current][i-1][j] + h[current][i+1][j]
			+ h[current][i][j-1] + h[current][i][j+1]);
			}
		}
	temp = current; // swap values of current and next
	current = next;
	next = temp;
	}
	end = omp_get_wtime();	//end time measurement
	sTime = end-start;
	for(i=0; i < N; i += (N/10)){
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",h[0][i][j]);
     		}
	}
	printf("\n");
	printf("Time of computation: %f seconds\n", end-start);

	usleep(100000); 						// some delay appears necessary

	XClearWindow(display, win);  					// clear window for next drawing
	XSetForeground(display,gc,(long) 0xDC143C);  			// color of foreground (applies to object to be drawn)
	x=0;
	y=0;
	int blue, green, red, yellow, black, purple, pink, orange;
	for(i=0; i < N; i ++){
     		for(j=0; j < N; j ++) {
			
         		if(h[0][i][j] == 20.0){
			blue++;
			}else if(h[0][i][j] >20.0 && h[0][i][j] <30.){
			blue++;
			}else if(h[0][i][j] >=30.0 && h[0][i][j] <40.0){
			green++;
			}else{
			red++;
			}        		
     		}
	}
	//blue
	XSetForeground(display,gc,(long) 0x0000FF);
	XFillArc (display,win,gc,400,0,.05*blue,.05*blue,0,23040);
	XFlush(display);
	//green
	XSetForeground(display,gc,(long) 0x009933);
	XFillArc (display,win,gc,400,0,1*green,1*green,0,23040);
	XFlush(display);
	//red
	XSetForeground(display,gc,(long) 0xDC143C);
	XFillArc (display,win,gc,400,0,1*red,1*red,0,23040);
	XFlush(display);
	//XDrawPoint (display, win, gc, 400, 400); 			// draw point at location x, y in window
							
	//XFillArc (display,win,gc,400,400,50,50,0,23040);		// draw circle of size 50x50 at location 400,400
	
	XFlush(display);						// necessary to write to display
	
	usleep(10000000);						// provide a delay beween each drawing

	printf("\nSequential, parallelized\n");
	
	float p[2][N][N];
	for (x = 0; x < 2; x++) {
		for (j = 0; j < N; j++)
			for (k = 0; k < N; k++){
			p[x][j][k] = 20;
			if((x == 0 && j == 0 && k >= (N/10)*4 && k <= (N/10)*5)){
			p[x][j][k] = 100.0;
			}}
				
	}
	for(i=0; i < N; i += (N/10))
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",p[0][i][j]);
     	}
	printf("\n");
	
	#pragma omp parallel private(i, j, current, next, temp, iteration)
	{
	start = omp_get_wtime();
	current = 0;
	next = 1;
	#pragma omp schedule(dynamic, CHUNKSIZE) for nowait
	for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
		for (i = 1; i < N-1; i++){
			for (j = 1; j < N-1; j++){
			p[next][i][j] = 0.25 * (p[current][i-1][j] + p[current][i+1][j]
			+ p[current][i][j-1] + p[current][i][j+1]);
			}
		}
	temp = current; // swap values of current and next
	current = next;
	next = temp;
	}end = omp_get_wtime();	//end time measurement
}
	
	pTime = end-start;
	for(i=0; i < N; i += (N/10)){
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",p[0][i][j]);
     		}
	}
	printf("\n");
	printf("Time of parallel computation: %f seconds\n", end-start);
	
	int error = 0;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)   {
			if (h[0][i][j] != p[0][i][j]) error = -1;
		}
	}
	usleep(100000); 						// some delay appears necessary

	XClearWindow(display, win);  					// clear window for next drawing
	XSetForeground(display,gc,(long) 0xDC143C);  			// color of foreground (applies to object to be drawn)
	x=0;
	y=0;
	blue =0;
	green = 0;
	red = 0;
	for(i=0; i < N; i ++){
     		for(j=0; j < N; j ++) {
			
         		if(h[0][i][j] == 20.0){
			blue++;
			}else if(p[0][i][j] >20.0 && p[0][i][j] <30.){
			blue++;
			}else if(p[0][i][j] >=30.0 && p[0][i][j] <40.0){
			green++;
			}else{
			red++;
			}        		
     		}
	}
	//blue
	XSetForeground(display,gc,(long) 0x0000FF);
	XFillArc (display,win,gc,400,0,.05*blue,.05*blue,0,23040);
	XFlush(display);
	//green
	XSetForeground(display,gc,(long) 0x009933);
	XFillArc (display,win,gc,400,0,1*green,1*green,0,23040);
	XFlush(display);
	//red
	XSetForeground(display,gc,(long) 0xDC143C);
	XFillArc (display,win,gc,400,0,1*red,1*red,0,23040);
	XFlush(display);
	XFlush(display);						// necessary to write to display
	
	usleep(10000000);						// provide a delay beween each drawing
	if (error == -1) printf("ERROR, sequential and parallel versions give different answers\n");
	printf("Speed up factor: %f\n", sTime/pTime);
	return(0);
}

