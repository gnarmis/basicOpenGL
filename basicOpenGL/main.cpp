//
//  main.cpp
//  basicOpenGL
//
//  Created by Gursimran Singh on 1/28/12.
//  Copyright (c) 2012 Northwestern University. All rights reserved.
//

#include "basicOpenGL.h"
using namespace std;

//=====================
// DISPLAY GLOBAL VARIABLES: bad idea, but forced by GLUT's callback fcns.
//                   Do your best to keep the number of global vars low.
//=====================
static int nu_display_width = 640;	// global variables that hold current
static int nu_display_height = 640;	// display window position and size;
static int nu_display_xpos = 200;	// See getDisplayXXX() functions.
static int nu_display_ypos = 100;
static int nu_Anim_isOn = 1;        // ==1 to run animation, ==0 to pause.
// !DON'T MESS WITH nu_Anim_isOn; call runAnimTimer().
//========================
// Global vars for our application:

// (NONE)

int main (int argc, char * argv[])
{
    //--GLUT/openGL Setup-------------------
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH  );
    // set rendering mode to RGB (not RGBA) color,
    // single-buffering GLUT_SINGLE, or
    // double-buffering GLUT_DOUBLE, where you'd need to call
    // glutSwapBuffers() after drawing operations to show
    // on-screen what you draw)
    //  enable depth testing for 3D drawing ('Z-buffering')
	glutInitWindowSize(getDisplayWidth(), getDisplayHeight() );
    glutInitWindowPosition(getDisplayXpos(),getDisplayYpos() );
    // set display window size & position from global vars
	glutCreateWindow( "EECS351-1 Project A" ); // set window title-bar name
	glClearColor( 0, 0, 0, 1 );	    // 'cleared-screen' color: black (0,0,0)
	// Register our callback functions in GLUT:
	glutDisplayFunc( myDisplay );	// callback for display window redraw
	glutReshapeFunc( myReshape);    // callback to create or re-size window
	glutVisibilityFunc(myHidden);   // callback for display window cover/uncover
	glutKeyboardFunc( myKeyboard);  // callback for alpha-numeric keystroke
	glutSpecialFunc ( myKeySpecial);// callback for all others: arrow keys,etc.
	glutMouseFunc( myMouseClik );   // callback for mouse button events
	glutMotionFunc( myMouseMove );  // callback for mouse dragging events
    runAnimTimer(1);                // start our animation loop.
	glutMainLoop();	                // enter GLUT's event-handler; NEVER EXITS.
}

void myReshape( int width, int height )
//------------------------------------------------------------------------------
{
    // Called by GLUT on initial creation of the window, using the width,height
    // arguments given in glutInitWindowSize(); see main() fcn. ALSO called by GLUT
    // whenever you re-size the display window to a different width, height.
    nu_display_width    = width;    // save the current display size
    nu_display_height   = height;   // in these global vars.
    
    //-------------------- glViewport() usage:
    //
    //   ?What coordinates glVertex() calls will actually show up on-screen?
    //
    // BACKGROUND:
    // OpenGL transforms glVertex() three times before they reach the screen, first
    //  by the GL_MODELVIEW matrix (used to build 3D shapes and place your camera),
    //  then the GL_PROJECTION matrix (used to make a 2D image from 3D vertices),
    //  then by the 'Viewport' matrix (used to fit the 2D image onto the screen).
    //  !CHAOS!: ALL THREE matrices can change a glVertex call's on-screen location,
    //          and the windowing system may change display window size & location.
    //
    //      What goes where? How do we start?
    //
    // Fortunately, we can always rely on the CVV ('canonical view volume') as a
    // our reference, our anchor for all the coordinate systems we chain together
    // with these 3 matrices.
    //
    // The 'Canonical View Volume' is a simple axis-centered cube with corners at
    // (+/-1, +/-1, +/-1), defined at the output of the GL_PROJECTION matrix.
    // OpenGL 'clips' to this volume; it simply discards any part of any drawing
    // primitive that comes out of the GL_PROJECTION matrix with values outside
    // this CVV cube.  Any openGL drawing you see passed through this CVV.
    //
    // Initially, GL_MODELVIEW and GL_PROJECTION both hold 4x4 indentity matrices,
    // and (thanks to GLUT) the Viewport fills your display window with the CVV.
    // If your glVertex() calls specify (x,y,z) values within (+/-1, +/-1, +/-1)
    // (inside the CVV) you can make on-screen pictures with them.  If you mess up,
    // you can always return to this state by re-initializing the 3 matrices:
    //          glMatrixMode(GL_MODELVIEW); // select and
    //          glLoadIdentity();           // set to identity matrix.
    //          glMatrixMode(GL_PROJECTION);// select and
    //          glLoadIdentity();           // set to identity matrix.
    //          glViewport(0,0,screenWidth,screenHeight); // fill screen with CVV.
    //
    // The glViewport() command specifies the mapping from CVV to display-window
    // pixels, but its method is rather odd -- remember its arguments as:
    //          glViewport(int xLL, int yLL, int width, int height);
    // The (xLL,yLL) arguments specify the display window pixel address for the
    // CVV's (-1,-1) (lower-left) corner; any vertex that leaves the
    // GL_PROJECTION matrix with value (-1,-1) will appear at (xLL,yLL) on-screen.
    // The 'width' and 'height' specify how many pixels the entire CVV would
    // cover on-screen, and we see all that falls within the display window.
    //
    // The CVV is always square, but our display windows are not; remember that
    // contents of the always-square CVV don't look their best if squashed or
    // stretched to fill an entire non-square display window. Also, you're not
    // required to fill every pixel of the display window with openGL drawings,
    // and/or you don't have to show the entire CVV in your display window.  You'll
    // get the best looking results if your glViewport() call keeps width=height.
    // Try one of these (comment out all but one):
    //-------------------------------
    //  glViewport(0,0, width, width);	// viewport may extend past window TOP
    //-------------------------------
    //  glViewport(0,0,height,height);	// viewport may extent past window RIGHT
    //-------------------------------
    // Draw CVV in window's largest centered inscribed square:
    if(width > height)
    {	// WIDE window:  height sets viewport size, and
        glViewport((width-height)/2,0, height, height);
    }	// move origin rightwards to keep image centered in window.
    else
    {	// TALL window:  width sets viewport size, and
        glViewport(0,(height-width)/2, width, width);
        // need to origin upwards to keep image centered in window
    }		// viewport stays SQUARE, but extends to left and right of window
    //--------------------------------
    // or invent your own; Perhaps make a square CVV with the window fitted inside?
    // Or could you draw TWO viewports on-screen, side-by-side in the same window?
    //
	glutPostRedisplay();			// request redraw--we changed window size!
}

void myDisplay( void )
//------------------------------------------------------------------------------
// Registered as our GLUT display callback function.
// GLUT calls us whenever we need to re-draw the screen.
// OPENGL HINTS:
//	glRotated(123, 1,-2,1);	// rotate by 170 degrees around a 3D axis vector;
//	glScaled(0.2, 0.3, 0.4);	// shrink along x,y,z axes
//  glTranslated(0.1,0.3,0.5);	// translate along x,y,z axes.
{
    // Clear the screen (to change screen-clearing color, try glClearColor() )
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // clear the color and depth buffers. Now draw:
    
    // First, draw nested squares with NO transformations:
	glMatrixMode(GL_MODELVIEW);     // select the modeling matrix stack
	glLoadIdentity();               // set to 'do nothing';
	drawAxes();			            // draw simple +x,y,z axes.
    
    glColor3d(0.0, 1.0, 1.0);       // bright yellow
    drawText2D(rom24,-0.8,-0.8, "Try mouse click&drag, arrow keys, other keys.");
    drawText2D(rom24,-0.8,-0.9, "(Space bar, ESC, 'q' or 'Q' to quit)");
    
	glColor3d(1.0, 0.0, 0.0);       // and a RED square.
	drawSquareEdges();
    glScaled(0.9,0.9,0.9);       // Scale everything by 0.9 and
    glColor3d(0.0, 1.0, 0.0);       // draw a GREEN square;
    drawSquareEdges();
    glScaled(0.9,0.9,0.9);       // Scale AGAIN by 0.9 and
    glColor3d(0.0, 0.0, 1.0);       // draw a BLUE square.
    drawSquareEdges();
    
    // Draw a white box-spiral at upper left:
    glPushMatrix();             // SAVE current coordinate system,
    glTranslated(0.6, 0.6, 0.0);    // Move to the side a bit,
    glScaled(0.25, 0.25, 0.25);     // shrink to 1/4 prev. size,
    glColor3d(1.0, 1.0, 1.0);       // Choose white,
    drawSquareEdges();              // draw outermost square, then
    for(int i=0; i<10; i++)
    {
        glScaled(0.9, 0.9, 0.9); // shrink and
        glRotated(11.0, 0,0,1);     // rotate 11 degrees around z axis and
        drawSquareEdges();          // draw another square.
    }
    glPopMatrix();              // RESTORE previous coord. system
    glRotated(45.0, 0.0, 0.0, 1.0); // rotate 45 degrees around z axis,
    glColor3d(1.0,0.8,0.3);         // orange,
    drawText2D(helv12,0.2, 0.2, "See? text drawing");
    drawText2D(helv12,0.2, 0.1,   "doesn't rotate 45 degrees,");
    drawText2D(helv12,0.2, 0.0,   "but the starting position does");
    drawText2D(helv12,0.2,-0.1,   "for each of these lines of text .");
    // go back to drawing in CVV coords; restore identity matrix in GL_MODELVIEW
    glLoadIdentity();               // Now the screen spans +/-1,+/-1 again.
    
	//===============DRAWING DONE.
	glFlush();	                // do any and all pending openGL rendering.
	glutSwapBuffers();			// For double-buffering: show what we drew.
}

void myKeyboard(unsigned char key, int xw, int yw)
//------------------------------------------------------------------------------
// GLUT CALLBACK: Don't call this function in your program--GLUT does it every
// time user presses an alphanumeric key on the keyboard.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left)

{
    int xpos,ypos;  // mouse position in coords with origin at lower left.
    
    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)
    
	switch(key)
	{
		case 'A':       // User pressed the 'A' key...
		case 'a':
            cout << "that *IS* the 'A' key!\n";
            break;
		case ' ':		// User pressed the spacebar.
		case 27:		// User pressed the 'Esc'  key...
		case 'Q':		// User pressed the 'Q' key...
		case 'q':
			exit(0);    // Done! quit the program.
			break;
        default:
            cout << "I don't respond to the %c key.\n" << key << endl;
            break;
	}
    glColor3d(1.0, 1.0, 1.0);// And white-colored text on-screen:
    drawText2D(helv18,-0.5, 0.0,"Ordinary Key.");
    //===============DRAWING DONE.
    glFlush();	            // do any and all pending openGL rendering.
    glutSwapBuffers();	    // For double-buffering: show what we drew.
}

void myKeySpecial(int key, int xw, int yw)
//------------------------------------------------------------------------------
// Called by GLUT when users press 'arrow' keys, 'Fcn' keys, or any other
// non-alphanumeric key.  Search glut.h to find other #define literals for
// special keys, such as GLUT_KEY_F1, GLUT_KEY_UP, etc.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left!)/
{
    int xpos,ypos;      // mouse position in coords with origin at lower left.
    
    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)
	switch(key)
	{
		case GLUT_KEY_LEFT:		// left arrow key
            cout << "left-arrow key.\n";
			break;
		case GLUT_KEY_RIGHT:	// right arrow key
            cout << "right-arrow key.\n";
			break;
		case GLUT_KEY_DOWN:		// dn arrow key
            cout << "dn-arrow key.\n";
			break;
		case GLUT_KEY_UP:		// up arrow key
            cout << "up-arrow key.\n";
			break;
            // SEARCH glut.h for more arrow key #define statements.
		default:
			cout << "Special key; integer code value"<< (int)key << "\n)";
			break;
	}
    glColor3d(0.5, 1.0, 0.5);	// And yellow-colored text on-screen:
    drawText2D(rom24,-0.5, -0.1,"That's a SPECIAL key!");
    //===============DRAWING DONE.
    glFlush();	        // do any and all pending openGL rendering.
    glutSwapBuffers();	// For double-buffering: show what we drew.
}

void myMouseClik(int buttonID, int upDown, int xw, int yw)
//-----------------------------------------------------------------------------
// Called by GLUT on mouse button click or un-click
// When user clicks a mouse button,  buttonID== 0 for left mouse button,
//                              		    (== 1 for middle mouse button?)
//	                                 		 == 2 for right mouse button;
//	                        upDown  == 0 if mouse button was pressed down,
//	                                == 1 if mouse button released.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left!)
{
    int xpos,ypos;  // mouse position in coords with origin at lower left.
    
    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)
    
    cout << "click; buttonID=" << buttonID <<", upDown=" << upDown;
    cout << ", at xpos,ypos=(" << xpos <<"," << ypos << ")\n";
    
    glColor3d(0.5, 1.0, 0.5);	//Bright green text:
    drawText2D(rom24, 0.0, -0.1, "!MOUSE CLICK!");
    //===============DRAWING DONE.
    glFlush();	        // do any and all pending openGL rendering.
    glutSwapBuffers();	// For double-buffering: show what we drew.
}

void myMouseMove(int xw, int yw)
//-----------------------------------------------------------------------------
// Called by GLUT when user moves mouse while pressing any mouse button.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left)
{
    int xpos,ypos;  // mouse position in coords with origin at lower left.
    
    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)
    
    cout << ".";
    
    glColor3d(1.0, 0.5, 0.5);	//bright red text:
    drawText2D(rom24,0.0, -0.2, "!MOUSE MOVE!");
    //===============DRAWING DONE.
    glFlush();	        // do any and all pending openGL rendering.
    glutSwapBuffers();	// For double-buffering: show what we drew.
}

void runAnimTimer(int isOn)
//-----------------------------------------------------------------------------
// Call runAnimTimer(0) to STOP animation;
//      runAnimTimer(1) to START or CONTINUE it.
{
    nu_Anim_isOn = isOn;   // Update global var to the most-recent state;
    if(isOn == 1)          // Start or continue animation?
    {   //YES. ANIMATE!   register a GLUT timer callback:
        glutTimerFunc(NU_PAUSE, myTimer, 1);
        // This registers 'myTimer()' as a timer callback function.
        // GLUT will now pause NU_PAUSE milliseconds (longer if busy drawing),
        // call myTimer(1), and then 'un-register' the callback; it won't call
        // myTimer() again unless we register it again.
        // myTimer(1) checks to be SURE no other 'stop animation' requests
        // arrived (e.g. keyboard input, mouse input, visibility change, etc.)
        // while we waited.  If global variable nu_Anim_isOn is STILL 1,
        // it calls runAnimTimer(1) to continue animation.
    }
    // NO.  STOP animation-- don't register a GLUT timer for myTimer().
}

void myTimer (int value)
//------------------------------------------------------------------------------
// Don't call this function yourself; use runAnimTimer() instead!
// runAnimTimer(1) causes GLUT to wait NU_PAUSE milliseconds (longer if busy)
// and then call this function.
// TRICKY: We allow users to choose very long NU_PAUSE times to see what happens
// in animation.  During GLUT's wait time, user events (keyboard, mouse, or
// window cover/uncover) may make runAnimTimer(0) calls intended to STOP
// animation.  GLUT can't cancel a previously-issued GLUT timer callback, so
// in the TimerCallback() function we check global variable nu_Anim_isON.
{
    // GLUT finished waiting.
    if(value != nu_Anim_isOn)
        cout << "\n(Anim cancelled during GLUT wait)\n";
    if(nu_Anim_isOn==1)         // Go on with animation?
    {                           // YES.
        glutPostRedisplay();    // tell GLUT to re-draw the screen, and
        runAnimTimer(1);        // continue with the next time interval.
    }
    // NO. Do nothing. No further animation.
}

void myHidden (int isVisible)
//------------------------------------------------------------------------------
// Called by GLUT when our on-screen window gets covered or un-covered. When
// called, it turns off animation and re-display when our display window is
// covered or stowed (not visible). SEE ALSO: runAnimTimer() and myTimer()
{
    if(isVisible == GLUT_VISIBLE)  runAnimTimer(1); // Visible! YES! Animate!
    else runAnimTimer(0);                           // No. Stop animating.
}


void drawText2D(void * pFont, double x0, double y0, const char* pString)
//-----------------------------------------------------------------------------
//  Using the current color and specified font, write the text in 'pString' to
// the display window.  Place the lower left corner of the first line of text
//  at screen position x0,y0 in the current coordinate system.
//      Example:  DrawText2D(helv18, 0.3,0.4 ,"Hello!");
//
// Available fonts:
// helv10 (HELVETICA size 10)   rom10 (TIMES_ROMAN size 10)
// helv12 (HELVETICA size 12)   rom24 (TIMES_ROMAN size 24)
// helv18 (HELVETICA size 18)
{
    int i, imax;					// character counters
    
	glRasterPos2d(x0, y0);		// set text's lower-left corner position
	imax = 1023;				// limit the number of chars we print.
	for(i=0; pString[i] != '\0' && i<imax; i++)	// for each char,
	{
		glutBitmapCharacter(pFont, pString[i]);
	}
}

void drawAxes(void)
//------------------------------------------------------------------------------
// Draw small +x,+y,+z axes in red, green, blue:
{
	glColor3d(1.0, 0.0, 0.0);	// red
	glBegin(GL_LINES);
    glVertex3d(0.0,0.0,0.0);	// +x-axis
    glVertex3d(0.4,0.0,0.0);
	glEnd();
	glColor3d(0.0, 1.0, 0.0);	// green
	glBegin(GL_LINES);
    glVertex3d(0.0,0.0,0.0);	// +y-axis
    glVertex3d(0.0,0.4,0.0);
	glEnd();
	glColor3d(0.0, 0.5, 1.0);	// bright blue
	glBegin(GL_LINES);
    glVertex3d(0.0,0.0,0.0);	// +z-axis
    glVertex3d(0.0,0.0,0.4);
	glEnd();
}

// Read the current size and position of the graphics display window
//------------------------------------------------------------------------------
int getDisplayHeight(void) { return nu_display_height; }
int getDisplayWidth(void)  { return nu_display_width; }
int getDisplayXpos(void)   { return nu_display_xpos; }
int getDisplayYpos(void)   { return nu_display_ypos; }


void drawSquarePoints(void)
//------------------------------------------------------------------------------
// Draw a 2D square at points +/-1, +/-1. Note counter-clockwise order:
{
	glPointSize(3.0);
	glBegin(GL_POINTS);
    glVertex2d( 1.0,  1.0);
    glVertex2d(-1.0,  1.0);
    glVertex2d(-1.0, -1.0);
    glVertex2d( 1.0, -1.0);
	glEnd();
    
}

void drawSquareEdges(void)
//------------------------------------------------------------------------------
// Draw a 2D square at points +/-1, +/-1. Note counter-clockwise order:
{
	glBegin(GL_LINE_LOOP);
    glVertex2d( 1.0,  1.0);
    glVertex2d(-1.0,  1.0);
    glVertex2d(-1.0, -1.0);
    glVertex2d( 1.0, -1.0);
	glEnd();
    
}


void drawSquareFace(void)
//------------------------------------------------------------------------------
// Draw a 2D square at point +/-1, +/-1. Note counter-clockwise order:
{
	glBegin(GL_QUADS);
    glVertex2d( 1.0,  1.0);
    glVertex2d(-1.0,  1.0);
    glVertex2d(-1.0, -1.0);
    glVertex2d( 1.0, -1.0);
	glEnd();
}

