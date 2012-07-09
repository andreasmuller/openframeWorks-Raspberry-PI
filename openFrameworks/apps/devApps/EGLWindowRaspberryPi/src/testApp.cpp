#include "testApp.h"
#include <stdio.h>

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::setup()
{

	ofSetLogLevel( OF_LOG_VERBOSE );


    //ofSetVerticalSync(true);
	//ofSetFrameRate(60);


	cout << __PRETTY_FUNCTION__ << endl;
	
	GLint max_tex = -1; 
	glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_tex); 
	printf("Max texture is %d\n", (int)max_tex); 

}


//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::update()
{
	//cout << __PRETTY_FUNCTION__ << "  Fps: " << ofGetFrameRate() << "  " << ofGetFrameNum() << endl;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::draw()
{
	ofBackground( (int)ofRandom(255), (int)ofRandom(255), (int)ofRandom(255) );
    //ofBackgroundGradient(ofColor::gray,ofColor::black);

 // scale the colour based on the width
	float r=(float)rand()/(float)RAND_MAX;
	float g=(float)rand()/(float)RAND_MAX;
	float b=(float)rand()/(float)RAND_MAX;
	// set the clear colour
	glClearColor(r,g,b,1);
	// clear screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	ofSetColor( (int)ofRandom(255), (int)ofRandom(255), (int)ofRandom(255) );
	ofLine( ofRandom(400), ofRandom(400), ofRandom(400), ofRandom(400) );



	//cout << __PRETTY_FUNCTION__ << "  Fps: " << ofGetFrameRate() << "  " << ofGetFrameNum() << endl;
}


//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::keyPressed(int key){

    if (key == ' ')
    {

    }

}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::keyReleased(int key)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::mouseMoved(int x, int y )
{

}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::mouseDragged(int x, int y, int button)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::mousePressed(int x, int y, int button)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::mouseReleased(int x, int y, int button)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::windowResized(int w, int h)
{
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::gotMessage(ofMessage msg)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
