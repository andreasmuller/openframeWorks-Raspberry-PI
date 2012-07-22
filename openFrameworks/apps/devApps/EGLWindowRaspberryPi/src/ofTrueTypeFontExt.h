#pragma once

#include "ofMain.h"

class ofTrueTypeFontExt : public ofTrueTypeFont
{
	public:
			void drawTextureAtlas( float _x, float _y, float _w, float _h )
			{

				if( _w == 0.0f || _h == 0.0f )
				{
					_w = texAtlas.getWidth();
					_h = texAtlas.getHeight();					
				}

				texAtlas.draw( _x, _y, _w, _h );
			}
		
};
