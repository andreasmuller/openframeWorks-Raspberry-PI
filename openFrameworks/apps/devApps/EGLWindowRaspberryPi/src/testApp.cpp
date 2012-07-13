#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	for(int i = 0; i < 5; i++ ) { mouseIsDown[i] = false; }
	myFont.loadFont(ofToDataPath( "fonts/DIN.otf"), 24 );
	maxSavedChars = 10;
}

//--------------------------------------------------------------
void testApp::update(){

	string fullString = "";
	for( unsigned int i = 0; i < lastKey.size(); i++ )
	{
		fullString += ( lastKey.at(i) + " " );
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor( 255, 0, 0 );
	ofCircle( mouseX, mouseY, 30 );
	
	float tmpX = mouseX + 40;
	float tmpY = mouseY + 10;
	
	for( int i = 0; i < 5; i++ )
	{
		if( mouseIsDown[i] )
		{
			myFont.drawString( ofToString(i), tmpX, tmpY );
			tmpX += 20;
		}
	}
	
	string fullString = "";
	for( unsigned int i = 0; i < lastKey.size(); i++ )
	{
		fullString += ( lastKey.at(i) + " " );
	}
		
	ofSetColor( 255, 255, 255 );
	myFont.drawString( fullString, 20, 200 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if( key == OF_KEY_ESC ){
		lastKey.push_back( "ESC" );
	} 
	else  if( key == OF_KEY_RETURN ) {
		lastKey.push_back( "RETURN" );		
	}
	else  if( key == OF_KEY_UP ) {
		lastKey.push_back( "UP" );		
	}
	else  if( key == OF_KEY_DOWN ) {
		lastKey.push_back( "DOWN" );		
	}
	else  if( key == OF_KEY_LEFT ) {
		lastKey.push_back( "LEFT" );		
	}
	else  if( key == OF_KEY_RIGHT ) {
		lastKey.push_back( "RIGHT" );		
	}
	else  if( key == 32 ) {
		lastKey.push_back( "SPACE" );		
	}
	else {
		if( key > 32 && key < 127 )
		{
			// hackety hack.
			string tmpStr = " ";
			tmpStr.at(0) = key;
			lastKey.push_back( tmpStr );
		}
	}
		
	if( lastKey.size() > maxSavedChars ) {
		 lastKey.erase (lastKey.begin(),lastKey.begin()+1);
	}
		
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	mouseX = x;
	mouseY = y;	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	mouseIsDown[button] = true;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	mouseIsDown[button] = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}