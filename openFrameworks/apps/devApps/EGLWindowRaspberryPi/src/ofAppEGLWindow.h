#pragma once

#include "ofConstants.h"
#include "ofAppBaseWindow.h"
#include "ofEvents.h"
#include "ofTypes.h"
#include <map>

#include <cassert>

#include <EGL/egl.h>
#include <EGL/eglext.h>

//#define RASPBERRY_PI_DO_GLES2 0

#ifdef RASPBERRY_PI_DO_GLES2
    //#include <GLES2/gl2.h>
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

//class ofPoint;
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

	void setupOpenGL(int w, int h, int screenMode);
	
	void setDoubleBuffering(bool _bDoubleBuffered); 
	
	void initializeWindow();
	void runAppViaInfiniteLoop(ofBaseApp * appPtr);
	
	//note if you fail to set a compatible string the app will not launch
	void setGlutDisplayString(string str);

	void hideCursor();
	void showCursor();
	
	void setFullscreen(bool fullScreen);
	void toggleFullscreen();

	void setWindowTitle(string title);
	void setWindowPosition(int x, int y);
	void setWindowShape(int w, int h);

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

	void update();
	void display();
	void swapBuffers() const;

	int readMouse(int& _x, int& _y );
	void rotateMouseXY(ofOrientation orientation, int &x, int &y);

	/// @brief this must be implemented by the user, it is called only once when the
	/// window is created
	//virtual void initializeGL()=0;

	/// @brief destroy the surface if it exists
	void destroySurface();
	
	/// @brief this is the main function to create our surface
	/// @param[in] _x the left origin of the screen
	/// @param[in] _y the top origin of the screen
	/// @param[in] _w the width  of the screen
	/// @param[in] _h the height of the screen
	void makeSurface( uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h );

	string eglErrorToString( EGLint _err );

	/// @brief the current window width
	uint32_t m_width;
	/// @brief the current window height
	uint32_t m_height;
	/// @brief max width reported from EGL
	uint32_t m_maxWidth;
	/// @brief max heigh reported from EGL
	uint32_t m_maxHeight;

	/// @brief the display
	EGLDisplay m_display;
	/// @brief the surface
	EGLSurface m_surface;
	/// @brief the gl context
	EGLContext m_context;
	/// @brief our config either created by the user and passed in or
	/// a default one will be created for us
	//EGLconfig *m_config;
	ofEGLWindowConfig *m_config;

	/// @brief vc display manager element
	DISPMANX_ELEMENT_HANDLE_T m_dispmanElement;
	/// @brief vc display manager display structure
	DISPMANX_DISPLAY_HANDLE_T m_dispmanDisplay;
	/// @brief vc display manager update structure
	DISPMANX_UPDATE_HANDLE_T m_dispmanUpdate;

 	/// @brief flag to indicate if the surface has been created
 	bool m_activeSurface;

	/// @brief file pointer to the mouse, opened by the ctor closed by the dtor
	int m_mouse;
	int oldMouseX, oldMouseY;
	int mouseX, mouseY;

 	/// @brief flag to indicate if we upscale to full screen resolution
 	bool m_upscale;
	
	string displayString;
		 
};



/// @brief this file is a wrapper for the EGL config, used to set the attributes
/// for out EGL window.
/// The default ctor will make a useable config or the user can create their own
/// by default we will have RGBA 8 Bits with Depth 24 and GLES 2.0


class ofEGLWindowConfig
{
	public :
		/// @brief ctor will create a default attrib set
		ofEGLWindowConfig()
		{
			m_attributes[EGL_RED_SIZE]=8;
			m_attributes[EGL_GREEN_SIZE]=8;
			m_attributes[EGL_BLUE_SIZE]=8;
			m_attributes[EGL_ALPHA_SIZE]=8;
			m_attributes[EGL_SURFACE_TYPE]=EGL_WINDOW_BIT;
		}
		
		/// @brief dtor (doesn't do anything
		~ofEGLWindowConfig()
		{

		}
		
		/// @brief set the attrib / value pair, unfortunatly they are both ints and
		/// not enums so it's quite difficult to check apart from a hard coded range
		/// check based on the headers!
		/// @param[in] _attrib the attribute pair to set
		/// @param[in] _value the value of the attribute
		void setAttribute(EGLint _attrib,EGLint _value)
		{
//			assert(_attrib >= 0x3020 && _attrib <=0x3042);
			m_attributes[_attrib]=_value;
		}

		/// @brief sets the R,G,B,A bit size
		/// @param[in] _r the red bit depth
		/// @param[in] _g the green bit depth
		/// @param[in] _b the blue bit depth
		/// @param[in] _a the alpha bit depth
		void setRGBA(EGLint _r,EGLint _g, EGLint _b, EGLint _a)
		{
			m_attributes[EGL_RED_SIZE]=_r;
			m_attributes[EGL_GREEN_SIZE]=_g;
			m_attributes[EGL_BLUE_SIZE]=_b;
			m_attributes[EGL_ALPHA_SIZE]=_a;
		}

		/// @brief sets the depth buffer size
		/// @param[in] _d the depth
		void setDepth(EGLint _d)
		{
			m_attributes[EGL_DEPTH_SIZE]=_d;
		}
		/// @brief set the surface type
		/// @param[in] the surface type can be | combinations of
		/// EGL_WINDOW_BIT, EGL_PBUFFER_BIT, and EGL_PIXMAP_BIT
		void setSurface(EGLint _s)
		{
			m_attributes[EGL_SURFACE_TYPE]=_s;
		}
		/// @brief set the config for the current values of this class
		/// this must be called for any changes to take place
		/// @param[in] _display the active display created for the config
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
				result = eglChooseConfig(_display, &attribs[0], &m_config, 1, &numConfig);
				std::cout<<"got numCofig ="<<numConfig<<"\n";
				if( result==EGL_FALSE )
				{
					std::cerr<<"error setting config check your setting or if you have a valid display\n";
					exit(EXIT_FAILURE);
				}
				std::cout<<"choosing config\n";
		}
		/// @brief get the config create with choose to the client
		inline EGLConfig getConfig() 
		{
			return m_config;
		};

		private:
			std::map<EGLint,EGLint> m_attributes;
			EGLConfig m_config;

};
