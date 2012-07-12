#pragma once

#include "ofConstants.h"
#include "ofAppBaseWindow.h"
#include "ofEvents.h"
#include "ofTypes.h"
#include "ofLog.h"
#include <map>

#include <cassert>

#include <EGL/egl.h>
#include <EGL/eglext.h>

// just setting this is not going to work at the moment, we need a GL ES 2.0 openFrameworks Renderer as well
//#define RASPBERRY_PI_DO_GLES2 1

// Do we want to run under X11 or not?
//#define RASPBERRY_PI_X11 1


#ifdef RASPBERRY_PI_DO_GLES2
    #include <GLES2/gl2.h>
#else 
	#include <GLES/gl.h>
	//#define GL_GLEXT_PROTOTYPES
	#include <GLES/glext.h>
#endif

//#ifdef TARGET_RASPBERRY_PI
	#include <bcm_host.h>
	#include <unistd.h>
	#include <fcntl.h>
//#endif

#ifdef RASPBERRY_PI_X11
	#include  <X11/Xlib.h>
	#include  <X11/Xatom.h>
	#include  <X11/Xutil.h>
#endif



class ofBaseApp;
class ofEGLWindowConfig;

// The EGL code comes mostly from: http://jonmacey.blogspot.co.uk/2012/06/opengl-es-on-raspberry-pi-pt-3-creating.html

// Makefile.linux (or future raspberry pi specific one) in libs/openFrameworksCompiled/project/makefileCommon
// needs to have the references to -lGL and -lglew taken out, like so:
// SYSTEMLIBS += -lasound -lopenal -lsndfile -lvorbis -lFLAC -logg -lfreeimage -L/opt/vc/lib -lGLESv2 -lEGL -lm -lbcm_host

class ofAppEGLWindow : public ofAppBaseWindow {

public:

	ofAppEGLWindow();
	~ofAppEGLWindow();

	void 		setupOpenGL(int w, int h, int screenMode);
	
	void 		setDoubleBuffering(bool _bDoubleBuffered); 
	
	void 		initializeWindow();
	void 		runAppViaInfiniteLoop(ofBaseApp * appPtr);

	void 		hideCursor();
	void 		showCursor();
	
	void 		setFullscreen(bool fullScreen);
	void 		toggleFullscreen();

	void 		setWindowTitle(string title);
	void 		setWindowPosition(int x, int y);
	void 		setWindowShape(int w, int h);

	ofPoint		getWindowPosition();
	ofPoint		getWindowSize();
	ofPoint		getScreenSize();
	
	void			setOrientation(ofOrientation orientation);
	ofOrientation	getOrientation();
	
	int			getWidth();
	int			getHeight();	
	
	int			getWindowMode();

	int			getFrameNum();
	float		getFrameRate();
	double		getLastFrameTime();
	void		setFrameRate(float targetRate);

	void		enableSetupScreen();
	void		disableSetupScreen();

protected:

	void 		update();
	void 		display();
	void 		handleInput();
	void 		swapBuffers() const;

	int 		readMouse(int& _x, int& _y );
	void 		rotateMouseXY(ofOrientation orientation, int &x, int &y);

	/// this must be implemented by the user, it is called only once when the
	/// window is created
	//virtual void initializeGL()=0;

	/// destroy the surface if it exists
	void 		destroySurface();
	
	/// this is the main function to create our surface
	/// Param in: _x the left origin of the screen
	/// Param in: _y the top origin of the screen
	/// Param in: _w the width  of the screen
	/// Param in: _h the height of the screen
	void 		makeContext();

	string 		eglErrorToString( EGLint _err );

	int			windowMode;
	bool		bNewScreenMode;
	float		timeNow, timeThen, fps;
	int			nFramesForFPS;
	int			nFrameCount;
	int			buttonInUse;
	bool		bEnableSetupScreen;
	bool		bDoubleBuffered; 

	bool		bFrameRateSet;
	int 		millisForFrame;
	int 		prevMillis;
	int 		diffMillis;

	float 		frameRate;
	double		lastFrameTime;

	int			requestedWidth;
	int			requestedHeight;

	int 		nonFullScreenX;
	int 		nonFullScreenY;
	int			windowW;
	int			windowH;
	int         nFramesSinceWindowResized;

	ofOrientation	orientation;
	ofBaseApp*  ofAppPtr;

	/// screen/monitor width reported from EGL
	uint32_t 	monitorWidth, monitorHeight;

	// our source and destination rect for the screen
	VC_RECT_T	dstRect;
	VC_RECT_T	srcRect;

	/// the display
	EGLDisplay 	displayEGL;
	
	/// the surface
	EGLSurface 	surfaceEGL;
	
	/// the gl context
	EGLContext 	contextEGL;

	/// our config either created by the user and passed in or a default one will be created for us
	ofEGLWindowConfig *windowConfigEGL;

	EGLNativeWindowType __win;

	EGL_DISPMANX_WINDOW_T nativeWindow;

	/// vc display manager element
	DISPMANX_ELEMENT_HANDLE_T m_dispmanElement;
	/// vc display manager display structure
	DISPMANX_DISPLAY_HANDLE_T m_dispmanDisplay;
	/// vc display manager update structure
	DISPMANX_UPDATE_HANDLE_T m_dispmanUpdate;

#ifdef RASPBERRY_PI_X11
	Display* __x_display;
	Window __eventWin;
#endif


 	/// flag to indicate if the surface has been created
 	bool 		surfaceIsActive;

	/// file pointer to the mouse, opened by the ctor closed by the dtor
	int 		mouseFilePointer;
	int 		oldMouseX, oldMouseY;
	int 		mouseX, mouseY;		 

	bool 		__keys[256];
	int 		__mouse[3];
};



// The EGL code comes mostly from: http://jonmacey.blogspot.co.uk/2012/06/opengl-es-on-raspberry-pi-pt-3-creating.html

class ofEGLWindowConfig
{
	public :
		/// ctor will create a default attrib set
		ofEGLWindowConfig()
		{
			m_attributes[EGL_RED_SIZE]=8;
			m_attributes[EGL_GREEN_SIZE]=8;
			m_attributes[EGL_BLUE_SIZE]=8;
			m_attributes[EGL_ALPHA_SIZE]=8;
			m_attributes[EGL_SURFACE_TYPE]=EGL_WINDOW_BIT;
		}
		
		/// dtor (doesn't do anything
		~ofEGLWindowConfig()
		{

		}
		
		/// set the attrib / value pair, unfortunatly they are both ints and
		/// not enums so it's quite difficult to check apart from a hard coded range
		/// check based on the headers!
		/// Param in: _attrib the attribute pair to set
		/// Param in: _value the value of the attribute
		void setAttribute(EGLint _attrib,EGLint _value)
		{
			//assert(_attrib >= 0x3020 && _attrib <=0x3042);
			m_attributes[_attrib]=_value;
		}

		/// sets the R,G,B,A bit size
		/// Param in: _r the red bit depth
		/// Param in: _g the green bit depth
		/// Param in: _b the blue bit depth
		/// Param in: _a the alpha bit depth
		void setRGBA(EGLint _r,EGLint _g, EGLint _b, EGLint _a)
		{
			m_attributes[EGL_RED_SIZE]=_r;
			m_attributes[EGL_GREEN_SIZE]=_g;
			m_attributes[EGL_BLUE_SIZE]=_b;
			m_attributes[EGL_ALPHA_SIZE]=_a;
		}

		/// sets the depth buffer size
		/// Param in: _d the depth
		void setDepth(EGLint _d)
		{
			m_attributes[EGL_DEPTH_SIZE]=_d;
		}
		/// set the surface type
		/// Param in: the surface type can be | combinations of
		/// EGL_WINDOW_BIT, EGL_PBUFFER_BIT, and EGL_PIXMAP_BIT
		void setSurface(EGLint _s)
		{
			m_attributes[EGL_SURFACE_TYPE]=_s;
		}
		/// set the config for the current values of this class
		/// this must be called for any changes to take place
		/// Param in: _display the active display created for the config
		void chooseConfig(EGLDisplay _display)
		{
			std::map<EGLint,EGLint>::const_iterator it;

			std::vector <EGLint> attribs;
			for ( it=m_attributes.begin() ; it != m_attributes.end(); it++ )
			{
				attribs.push_back((*it).first);
				attribs.push_back((*it).second);
			}
			attribs.push_back(EGL_NONE);

			EGLBoolean result;
			EGLint numConfig;

			// get an appropriate EGL frame buffer configuration
			result = eglChooseConfig(_display, &attribs[0], &windowConfigEGL, 1, &numConfig);

			//std::cout<<"got numCofig ="<<numConfig<<"\n";
			if( result==EGL_FALSE )
			{
				std::cerr<<"error setting config check your setting or if you have a valid display\n";
				exit(EXIT_FAILURE);
			}
		}
		/// get the config create with choose to the client
		inline EGLConfig getConfig() 
		{
			return windowConfigEGL;
		};

		private:
			std::map<EGLint,EGLint> m_attributes;
			EGLConfig windowConfigEGL;

};
