#include "ofAppEGLWindow.h"
#include "ofBaseApp.h"
#include "ofEvents.h"
#include "ofUtils.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofConstants.h"

#ifdef TARGET_WIN32
#endif

#ifdef TARGET_OSX
#endif


//----------------------------------------------------------
ofAppEGLWindow::ofAppEGLWindow(){
	timeNow				= 0;
	timeThen			= 0;
	fps					= 60.0; //give a realistic starting value - win32 issues
	frameRate			= 60.0;
	windowMode			= OF_WINDOW;
	bNewScreenMode		= true;
	nFramesForFPS		= 0;
	nFramesSinceWindowResized = 0;
	nFrameCount			= 0;
	buttonInUse			= 0;
	bEnableSetupScreen	= true;
	bFrameRateSet		= false;
	millisForFrame		= 0;
	prevMillis			= 0;
	diffMillis			= 0;
	requestedWidth		= 0;
	requestedHeight		= 0;
	nonFullScreenX		= -1;
	nonFullScreenY		= -1;
	lastFrameTime		= 0.0;
	orientation			= OF_ORIENTATION_DEFAULT;
	bDoubleBuffered = true; // LIA

	windowConfigEGL = new ofEGLWindowConfig();

	//windowConfigEGL->setRGBA(5,6,5,0); // TODO: This is not possible yet.
	windowConfigEGL->setDepth( 16 );	 // TODO: This gets set to 24, maybe 32 bit color and 16 bit depth are not compatible?

}

//------------------------------------------------------------
ofAppEGLWindow::~ofAppEGLWindow()
{
	destroySurface();
	// release the mouse
	close(mouseFilePointer);
}

void ofAppEGLWindow::setDoubleBuffering(bool _bDoubleBuffered){ 
	bDoubleBuffered = _bDoubleBuffered;
}

//------------------------------------------------------------
void ofAppEGLWindow::setupOpenGL(int w, int h, int screenMode){

	atexit( bcm_host_deinit);
	bcm_host_init();

	// toggle we don't yet have an active surface
	surfaceIsActive = false;

	// set our display values to 0 (not once ported to cx11 will use nullptr but
	// current pi default compiler doesn't support it yet
	displayEGL = 0;
	contextEGL = 0;
	surfaceEGL = 0;

	// now find the max display size (we will use this later to assert if the user
	// defined sizes are in the correct bounds
	int32_t success = 0;
	success = graphics_get_display_size(0 , &fullScreenWidth, &fullScreenHeight);
	if ( success < 0 ) 
	{
		cout << __PRETTY_FUNCTION__ << " Was not able to read display size with graphics_get_display_size" << endl;
	}
	ofLogNotice() << "Screen max width,height " << fullScreenWidth << ", " << fullScreenHeight << endl;


	windowW = w;
	windowH = h;
	windowMode = screenMode;

	// if we have a user defined config we will use that else we need to create one
	//if (_config == 0) { std::cout<<"making new config\n"; windowConfigEGL= new EGLconfig(); }
	//else { windowConfigEGL=_config; }

	// this code actually creates the surface
	makeSurface(0,0,windowW,windowH);	 


	printf("-- From ofAppEGLWindow --------------------------------\n"); 
	// query egl-specific strings
	char *egl_vendor 	= (char *)eglQueryString(displayEGL, EGL_VENDOR);
	char *egl_version 	= (char *)eglQueryString(displayEGL, EGL_VERSION);
	char *egl_apis 		= (char *)eglQueryString(displayEGL, EGL_CLIENT_APIS);
	char *egl_exts 		= (char *)eglQueryString(displayEGL, EGL_EXTENSIONS);

	printf("EGL\n");
	printf("  Vendor: %s\n", egl_vendor);
	printf("  Version: %s\n", egl_version);
	printf("  Client APIs: %s\n", egl_apis);
	//printf("  Extensions: %s\n", egl_exts);

	char *vendor 	= (char *)glGetString(GL_VENDOR);
	char *renderer 	= (char *)glGetString(GL_RENDERER);
	char *version 	= (char *)glGetString(GL_VERSION);

	printf("OpenGL ES\n");
	printf("  Vendor: %s\n", vendor);
	printf("  Renderer: %s\n", renderer);
	printf("  Version: %s\n", version);

	GLint redBits, greenBits, blueBits, alphaBiths, depthBits ;

	glGetIntegerv (GL_RED_BITS, &redBits);
	glGetIntegerv (GL_GREEN_BITS, &greenBits);
	glGetIntegerv (GL_BLUE_BITS, &blueBits);
	glGetIntegerv (GL_ALPHA_BITS, &alphaBiths);	
	glGetIntegerv(GL_DEPTH_BITS, &depthBits);

	printf("\nContext color RGBA: %d,%d,%d,%d   depth: %d \n", redBits, greenBits, redBits, alphaBiths, depthBits );

	printf("--------------------------------------------------------\n");


	bNewScreenMode = true;
}

//------------------------------------------------------------
void ofAppEGLWindow::initializeWindow(){

	// For the time being we will handle mouse input by reading the mouse every frame, TODO: find a better solution
	oldMouseX = 0;
	oldMouseY = 0;
	mouseX = 0; 
	mouseY = 0;

	mouseFilePointer = open("/dev/input/mouse0",O_RDONLY|O_NONBLOCK);

}

//------------------------------------------------------------
void ofAppEGLWindow::runAppViaInfiniteLoop(ofBaseApp * appPtr){

	cout << __PRETTY_FUNCTION__ << endl;

	ofAppPtr = appPtr;

	ofNotifySetup();
	ofNotifyUpdate();

	bool quit = false;
	while(!quit)
	{
		update();
		display();

	}
}

//------------------------------------------------------------
void ofAppEGLWindow::update()
{
	handleInput();

	if (nFrameCount != 0 && bFrameRateSet == true){
		diffMillis = ofGetElapsedTimeMillis() - prevMillis;
		if (diffMillis > millisForFrame){
			; // we do nothing, we are already slower than target frame
		} else {
			int waitMillis = millisForFrame - diffMillis;
			#ifdef TARGET_WIN32
				Sleep(waitMillis);         //windows sleep in milliseconds
			#else
				usleep(waitMillis * 1000);   //mac sleep in microseconds - cooler :)
			#endif
		}
	}
	prevMillis = ofGetElapsedTimeMillis(); // you have to measure here

    // -------------- fps calculation:
	// theo - please don't mess with this without letting me know.
	// there was some very strange issues with doing ( timeNow-timeThen ) producing different values to: double diff = timeNow-timeThen;
	// http://www.openframeworks.cc/forum/viewtopic.php?f=7&t=1892&p=11166#p11166

	timeNow = ofGetElapsedTimef();
	double diff = timeNow-timeThen;
	if( diff  > 0.00001 ){
		fps			= 1.0 / diff;
		frameRate	*= 0.9f;
		frameRate	+= 0.1f*fps;
	 }
	 lastFrameTime	= diff;
	 timeThen		= timeNow;
  	// --------------

	ofNotifyUpdate();
}

//------------------------------------------------------------
void ofAppEGLWindow::handleInput()
{
	int mouseButtons = readMouse( mouseX, mouseY );
	//cout << mouseX << ", " << mouseY << "  buttons: " << mouseButtons << endl;

	mouseY = windowH - mouseY;

	// Hack for now, if mouse position has changed, send a mouse moved to OF
	if( oldMouseX != mouseX || oldMouseY != mouseY )
	{
		int rotatedX = mouseX;
		int rotatedY = mouseY;
		rotateMouseXY(orientation, rotatedX, rotatedY);

		if (nFrameCount > 0){
			ofNotifyMouseMoved(rotatedX, rotatedY);
		}

		oldMouseX = mouseX;
		oldMouseY = mouseY;		
	}

}

//------------------------------------------------------------
void ofAppEGLWindow::display(void){

	if (windowMode != OF_GAME_MODE){
		if ( bNewScreenMode ){
			if( windowMode == OF_FULLSCREEN){

				//----------------------------------------------------
				// before we go fullscreen, take a snapshot of where we are:
				nonFullScreenX = -1; //glutGet(GLUT_WINDOW_X);
				nonFullScreenY = -1; //glutGet(GLUT_WINDOW_Y);
				//----------------------------------------------------
				//glutFullScreen();

			}else if( windowMode == OF_WINDOW ){

				//glutReshapeWindow(requestedWidth, requestedHeight);
				//----------------------------------------------------
				// if we have recorded the screen posion, put it there
				// if not, better to let the system do it (and put it where it wants)
				if (nFrameCount > 0){
					//glutPositionWindow(nonFullScreenX,nonFullScreenY);
				}

			}
			bNewScreenMode = false;
		}
	}

	ofViewport(0, 0, windowW, windowH);

	float * bgPtr = ofBgColorPtr();
	bool bClearAuto = ofbClearBg();

	if ( bClearAuto == true || nFrameCount < 3){
		ofClear(bgPtr[0]*255,bgPtr[1]*255,bgPtr[2]*255, bgPtr[3]*255);
	}

	if( bEnableSetupScreen )ofSetupScreen();

	ofNotifyDraw();

    
	if (bClearAuto == false){
		// in accum mode resizing a window is BAD, so we clear on resize events.
		if (nFramesSinceWindowResized < 3){
			ofClear(bgPtr[0]*255,bgPtr[1]*255,bgPtr[2]*255, bgPtr[3]*255);
		}
	}
	if(bDoubleBuffered){
		swapBuffers();
	} else{
		glFlush();
	}
    
    nFramesSinceWindowResized++;
	nFrameCount++;		// increase the overall frame count

	//cout << __PRETTY_FUNCTION__ << endl;
}

//------------------------------------------------------------
void ofAppEGLWindow::swapBuffers() const {
	eglSwapBuffers(displayEGL, surfaceEGL);
}

//------------------------------------------------------------
float ofAppEGLWindow::getFrameRate(){
	return frameRate;
}

//------------------------------------------------------------
double ofAppEGLWindow::getLastFrameTime(){
	return lastFrameTime;
}

//------------------------------------------------------------
int ofAppEGLWindow::getFrameNum(){
	return nFrameCount;
}

//------------------------------------------------------------
void ofAppEGLWindow::setWindowTitle(string title){
	ofLogNotice() << " ofAppEGLWindow::setWindowTitle not yet implemented.";
}

//------------------------------------------------------------
ofPoint ofAppEGLWindow::getWindowSize(){
	return ofPoint(windowW, windowH,0);
}

//------------------------------------------------------------
ofPoint ofAppEGLWindow::getWindowPosition(){
	int x = -1;
	int y = -1;
	return ofPoint(x,y,0);
}

//------------------------------------------------------------
ofPoint ofAppEGLWindow::getScreenSize(){
	return ofPoint(fullScreenWidth, fullScreenHeight,0);
}

//------------------------------------------------------------
int ofAppEGLWindow::getWidth(){
	if( orientation == OF_ORIENTATION_DEFAULT || orientation == OF_ORIENTATION_180 ){
		return windowW;
	}
	return windowH;
}

//------------------------------------------------------------
int ofAppEGLWindow::getHeight(){
	if( orientation == OF_ORIENTATION_DEFAULT || orientation == OF_ORIENTATION_180 ){
		return windowH;
	}
	return windowW;
}

//------------------------------------------------------------
void ofAppEGLWindow::setOrientation(ofOrientation orientationIn){
	orientation = orientationIn;
}

//------------------------------------------------------------
ofOrientation ofAppEGLWindow::getOrientation(){
	return orientation;
}

//------------------------------------------------------------
void ofAppEGLWindow::setWindowPosition(int x, int y){
	//glutPositionWindow(x,y);
}

//------------------------------------------------------------
void ofAppEGLWindow::setWindowShape(int w, int h){
	//glutReshapeWindow(w, h);
	// this is useful, esp if we are in the first frame (setup):
	requestedWidth  = w;
	requestedHeight = h;
}

//------------------------------------------------------------
void ofAppEGLWindow::hideCursor(){

}

//------------------------------------------------------------
void ofAppEGLWindow::showCursor(){

}

//------------------------------------------------------------
void ofAppEGLWindow::setFrameRate(float targetRate){
	// given this FPS, what is the amount of millis per frame
	// that should elapse?

	// --- > f / s

	if (targetRate == 0){
		bFrameRateSet = false;
		return;
	}

	bFrameRateSet 			= true;
	float durationOfFrame 	= 1.0f / (float)targetRate;
	millisForFrame 			= (int)(1000.0f * durationOfFrame);

	frameRate				= targetRate;

}

//------------------------------------------------------------
int ofAppEGLWindow::getWindowMode(){
	return windowMode;
}

//------------------------------------------------------------
void ofAppEGLWindow::toggleFullscreen(){
	if( windowMode == OF_GAME_MODE)return;

	if( windowMode == OF_WINDOW ){
		windowMode = OF_FULLSCREEN;
	}else{
		windowMode = OF_WINDOW;
	}

	bNewScreenMode = true;
}

//------------------------------------------------------------
void ofAppEGLWindow::setFullscreen(bool fullscreen){
    if( windowMode == OF_GAME_MODE)return;

    if(fullscreen && windowMode != OF_FULLSCREEN){
        bNewScreenMode  = true;
        windowMode      = OF_FULLSCREEN;
    }else if(!fullscreen && windowMode != OF_WINDOW) {
        bNewScreenMode  = true;
        windowMode      = OF_WINDOW;
    }
}

//------------------------------------------------------------
void ofAppEGLWindow::enableSetupScreen(){
	bEnableSetupScreen = true;
}

//------------------------------------------------------------
void ofAppEGLWindow::disableSetupScreen(){
	bEnableSetupScreen = false;
}


//------------------------------------------------------------
void rotateMouseXY(ofOrientation orientation, int &x, int &y) {
	int savedY;
	switch(orientation) {
		case OF_ORIENTATION_180:
			x = ofGetWidth() - x;
			y = ofGetHeight() - y;
			break;

		case OF_ORIENTATION_90_RIGHT:
			savedY = y;
			y = x;
			x = ofGetWidth()-savedY;
			break;

		case OF_ORIENTATION_90_LEFT:
			savedY = y;
			y = ofGetHeight() - x;
			x = savedY;
			break;

		case OF_ORIENTATION_DEFAULT:
		default:
			break;
	}
}

// ----------------------------------------------------------------------------
void ofAppEGLWindow::makeSurface(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h)
{
	//cout << __PRETTY_FUNCTION__ << " x,y: " << _x << ", " << _y << "  w,h: " << _w << ", " << _h << endl;

	// this code does the main window creation
	EGLBoolean result;

	static EGL_DISPMANX_WINDOW_T nativeWindow;
	// our source and destination rect for the screen
	VC_RECT_T dstRect;
	VC_RECT_T srcRect;


#ifdef RASPBERRY_PI_DO_GLES2
     static EGLint const context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
#else 
    static EGLint const context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 1,
        EGL_NONE
    };
#endif

	// get an EGL display connection
	displayEGL = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(displayEGL == EGL_NO_DISPLAY)
	{
		ofLogError() << "ofAppEGLWindow::makeSurface error getting display";
		exit(EXIT_FAILURE);
	}
	// initialize the EGL display connection
	int major,minor;

	result = eglInitialize(displayEGL, &major, &minor);
	ofLogNotice() << "EGL init version "<<major<<"."<<minor<<"\n";
	if(result == EGL_FALSE)
	{
		ofLogError() << "ofAppEGLWindow::makeSurface error initialising display";
		exit(EXIT_FAILURE);
	}
	// get our config from the config class
	windowConfigEGL->chooseConfig(displayEGL);
	EGLConfig config = windowConfigEGL->getConfig();

	// bind the OpenGL API to the EGL
	result = eglBindAPI(EGL_OPENGL_ES_API);
	if(result == EGL_FALSE)
	{
		ofLogError() << "ofAppEGLWindow::makeSurface error binding API";
		exit(EXIT_FAILURE);
	}
	// create an EGL rendering context
	contextEGL = eglCreateContext(displayEGL, config, EGL_NO_CONTEXT, context_attributes );
	if(contextEGL == EGL_NO_CONTEXT)
	{
		ofLogError() << "ofAppEGLWindow::makeSurface could not get a valid context";
		exit(EXIT_FAILURE);
	}

	// create an EGL window surface the way this works is we set the dimensions of the srec
	// and destination rectangles.
	// if these are the same size there is no scaling, else the window will auto scale

	dstRect.x = _x;
	dstRect.y = _y;

	// If we are in OF_WINDOW mode, src and dst rects are the same, 
	// otherwise stretch result to full screen, presumably this is done by the scaler 
	// at no performance cost, but look into this.
	if( windowMode == OF_WINDOW ) 
	{
		dstRect.width = _w;
		dstRect.height = _h;
	}
	else
	{
		dstRect.width = fullScreenWidth;
		dstRect.height = fullScreenHeight;
	}

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.width = _w << 16;
	srcRect.height = _h << 16;

	// whilst this is mostly taken from demos I will try to explain what it does
	// there are very few documents on this ;-0
	// open our display with 0 being the first display, there are also some other versions
	// of this function where we can pass in a mode however the mode is not documented as
	// far as I can see
  	m_dispmanDisplay = vc_dispmanx_display_open(0);
  	
  	// now we signal to the video core we are going to start updating the config
	m_dispmanUpdate = vc_dispmanx_update_start(0);

	// this is the main setup function where we add an element to the display, this is filled in
	// to the src / dst rectangles
	m_dispmanElement = vc_dispmanx_element_add ( m_dispmanUpdate, m_dispmanDisplay,
		0, &dstRect, 0,&srcRect, DISPMANX_PROTECTION_NONE, 0 ,0,DISPMANX_NO_ROTATE);

	// now we have created this element we pass it to the native window structure ready
	// no create our new EGL surface
	nativeWindow.element = m_dispmanElement;
	nativeWindow.width  = _w;
	nativeWindow.height = _h;

	// we now tell the vc we have finished our update
	vc_dispmanx_update_submit_sync( m_dispmanUpdate );

	// finally we can create a new surface using this config and window
	surfaceEGL = eglCreateWindowSurface( displayEGL, config, &nativeWindow, NULL );

	if ( surfaceEGL == EGL_NO_SURFACE )
	{
		ofLogError() << "Error!  surfaceEGL == EGL_NO_SURFACE, eglCreateWindowSurface " << endl;
		exit(EXIT_FAILURE);		
	}

	// connect the context to the surface
	result = eglMakeCurrent(displayEGL, surfaceEGL, surfaceEGL, contextEGL);

	if ( result == EGL_FALSE )
	{
		ofLogError() << "Error!  result == EGL_FALSE, eglMakeCurrent " << endl;
		exit(EXIT_FAILURE);		
	}

//	assert(EGL_FALSE != result);
	surfaceIsActive = true;

}

//------------------------------------------------------------
void ofAppEGLWindow::destroySurface()
{
	if(surfaceIsActive == true)
	{
		eglSwapBuffers(displayEGL, surfaceEGL);
		// here we free up the context and display we made earlier
		eglMakeCurrent( displayEGL, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
		eglDestroySurface( displayEGL, surfaceEGL );
		eglDestroyContext( displayEGL, contextEGL );
		eglTerminate( displayEGL );
		surfaceIsActive=false;
	}
}

//------------------------------------------------------------
int ofAppEGLWindow::readMouse(int &_x, int &_y)
{
	static int x=windowW, y=windowH;
	const int XSIGN = 1<<4, YSIGN = 1<<5;
	if (mouseFilePointer>=0)
	{
		struct {char buttons, dx, dy; } m;
		while (1)
		{
			int bytes = read(mouseFilePointer, &m, sizeof m);

			if (bytes < (int)sizeof m) goto _exit;
			if (m.buttons&8)
			{
				break; // This bit should always be set
			}
			read(mouseFilePointer, &m, 1); // Try to sync up again
		}



	if (m.buttons&3)
		return m.buttons&3;
	x+=m.dx;
	y+=m.dy;
	if (m.buttons&XSIGN)
		x-=256;
	if (m.buttons&YSIGN)
		y-=256;
	if (x<0) x=0;
	if (y<0) y=0;
	if (x>windowW) x=windowW;
	if (y>windowH) y=windowH;
	}

	_exit:
	if (x) _x = x;
	if (y) _y = y;
	return 0;
}

//------------------------------------------------------------
void ofAppEGLWindow::rotateMouseXY(ofOrientation orientation, int &x, int &y) {
	int savedY;
	switch(orientation) {
		case OF_ORIENTATION_180:
			x = ofGetWidth() - x;
			y = ofGetHeight() - y;
			break;

		case OF_ORIENTATION_90_RIGHT:
			savedY = y;
			y = x;
			x = ofGetWidth()-savedY;
			break;

		case OF_ORIENTATION_90_LEFT:
			savedY = y;
			y = ofGetHeight() - x;
			x = savedY;
			break;

		case OF_ORIENTATION_DEFAULT:
		default:
			break;
	}
}


//------------------------------------------------------------
string ofAppEGLWindow::eglErrorToString( EGLint _err )
{

	if( _err == EGL_SUCCESS ) 					{ return "EGL_SUCCESS"; }
	//else if( _err == EGL_NO_SURFACE ) 			{ return "EGL_NO_SURFACE"; }
	else if( _err == EGL_BAD_DISPLAY ) 			{ return "EGL_BAD_DISPLAY"; }
	else if( _err == EGL_NOT_INITIALIZED ) 		{ return "EGL_NOT_INITIALIZED"; }
	else if( _err == EGL_BAD_CONFIG ) 			{ return "EGL_BAD_CONFIG"; }
	else if( _err == EGL_BAD_NATIVE_WINDOW ) 	{ return "EGL_BAD_NATIVE_WINDOW"; }
	else if( _err == EGL_BAD_ATTRIBUTE ) 		{ return "EGL_BAD_ATTRIBUTE"; }
	else if( _err == EGL_BAD_ALLOC ) 			{ return "EGL_BAD_ALLOC"; }
	else if( _err == EGL_BAD_MATCH ) 			{ return "EGL_BAD_MATCH"; }
	else if( _err == EGL_BAD_ACCESS ) 			{ return "EGL_BAD_ACCESS"; }
	else if( _err == EGL_BAD_CONTEXT ) 			{ return "EGL_BAD_CONTEXT"; }
	else if( _err == EGL_BAD_CURRENT_SURFACE ) 	{ return "EGL_BAD_CURRENT_SURFACE"; }
	else if( _err == EGL_BAD_SURFACE ) 			{ return "EGL_BAD_SURFACE"; }
	else if( _err == EGL_BAD_PARAMETER ) 		{ return "EGL_BAD_PARAMETER"; }
	else if( _err == EGL_BAD_NATIVE_PIXMAP ) 	{ return "EGL_BAD_NATIVE_PIXMAP"; }						
	else  { 
		return "*** Unknown EGL Error ***"; 
	}

}


/*
//------------------------------------------------------------
void ofAppEGLWindow::mouse_cb(int button, int state, int x, int y) {
	rotateMouseXY(orientation, x, y);

	if (nFrameCount > 0){
		if (state == GLUT_DOWN) {
			ofNotifyMousePressed(x, y, button);
		} else if (state == GLUT_UP) {
			ofNotifyMouseReleased(x, y, button);
		}

		buttonInUse = button;
	}
}

//------------------------------------------------------------
void ofAppEGLWindow::motion_cb(int x, int y) {
	rotateMouseXY(orientation, x, y);

	if (nFrameCount > 0){
		ofNotifyMouseDragged(x, y, buttonInUse);
	}

}

//------------------------------------------------------------
void ofAppEGLWindow::passive_motion_cb(int x, int y) {
	rotateMouseXY(orientation, x, y);

	if (nFrameCount > 0){
		ofNotifyMouseMoved(x, y);
	}
}
*/

/*
//------------------------------------------------------------
void ofAppEGLWindow::dragEvent(char ** names, int howManyFiles, int dragX, int dragY){

	// TODO: we need position info on mac passed through
	ofDragInfo info;
	info.position.x = dragX;
	info.position.y = ofGetHeight()-dragY;

	for (int i = 0; i < howManyFiles; i++){
		string temp = string(names[i]);
		info.files.push_back(temp);
	}

	ofNotifyDragEvent(info);
}
*/

/*
//------------------------------------------------------------
void ofAppEGLWindow::idle_cb(void) {


	//	thanks to jorge for the fix:
	//	http://www.openframeworks.cc/forum/viewtopic.php?t=515&highlight=frame+rate

	if (nFrameCount != 0 && bFrameRateSet == true){
		diffMillis = ofGetElapsedTimeMillis() - prevMillis;
		if (diffMillis > millisForFrame){
			; // we do nothing, we are already slower than target frame
		} else {
			int waitMillis = millisForFrame - diffMillis;
			#ifdef TARGET_WIN32
				Sleep(waitMillis);         //windows sleep in milliseconds
			#else
				usleep(waitMillis * 1000);   //mac sleep in microseconds - cooler :)
			#endif
		}
	}
	prevMillis = ofGetElapsedTimeMillis(); // you have to measure here

    // -------------- fps calculation:
	// theo - now moved from display to idle_cb
	// discuss here: http://github.com/openframeworks/openFrameworks/issues/labels/0062#issue/187
	//
	//
	// theo - please don't mess with this without letting me know.
	// there was some very strange issues with doing ( timeNow-timeThen ) producing different values to: double diff = timeNow-timeThen;
	// http://www.openframeworks.cc/forum/viewtopic.php?f=7&t=1892&p=11166#p11166

	timeNow = ofGetElapsedTimef();
	double diff = timeNow-timeThen;
	if( diff  > 0.00001 ){
		fps			= 1.0 / diff;
		frameRate	*= 0.9f;
		frameRate	+= 0.1f*fps;
	 }
	 lastFrameTime	= diff;
	 timeThen		= timeNow;
  	// --------------

	ofNotifyUpdate();

	glutPostRedisplay();

}


//------------------------------------------------------------
void ofAppEGLWindow::keyboard_cb(unsigned char key, int x, int y) {
	ofNotifyKeyPressed(key);
}

//------------------------------------------------------------
void ofAppEGLWindow::keyboard_up_cb(unsigned char key, int x, int y){
	ofNotifyKeyReleased(key);
}

//------------------------------------------------------
void ofAppEGLWindow::special_key_cb(int key, int x, int y) {
	ofNotifyKeyPressed(key | OF_KEY_MODIFIER);
}

//------------------------------------------------------------
void ofAppEGLWindow::special_key_up_cb(int key, int x, int y) {
	ofNotifyKeyReleased(key | OF_KEY_MODIFIER);
}

//------------------------------------------------------------
void ofAppEGLWindow::resize_cb(int w, int h) {
	windowW = w;
	windowH = h;

	ofNotifyWindowResized(w, h);

	nFramesSinceWindowResized = 0;
}

void ofAppEGLWindow::entry_cb( int state ) {
	
	ofNotifyWindowEntry( state );
	
}
	*/

