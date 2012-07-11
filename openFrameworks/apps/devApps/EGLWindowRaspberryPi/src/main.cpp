

//#define COMMAND_LINE_ONLY

#include "ofMain.h"
#include "testApp.h"

//#ifdef COMMAND_LINE_ONLY
//	#include "Utils/ofAppNoWindow.h"
//#else
	#include "ofAppEGLWindow.h"
//#endif




//-------------------------------------------------------------------------------------------------------------------------------------
//
int main(  int argc, char *argv[]  )
{

//#ifdef COMMAND_LINE_ONLY
//	ofAppNoWindow window;
//#else
	ofAppEGLWindow window;
//#endif

	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);
	testApp * app = new testApp;
	ofRunApp( app );
        
}
