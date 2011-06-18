/*
 *  Sprite.h
 *  NinjaTraining
 *
 *  Created by Tommaso Checchi on 4/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Sprite_h__
#define Sprite_h__

#include "dojo/dojo_common_header.h"

#include "dojo/Array.h"

#include "dojo/Texture.h"
#include "dojo/Vector.h"
#include "dojo/Renderable.h"
#include "dojo/FrameSet.h"
#include "dojo/Color.h"
#include "dojo/Render.h"
#include "dojo/AnimatedQuad.h"

namespace Dojo
{	
	class Sprite : public AnimatedQuad
	{	
		
	public:		
				
		typedef Array<Animation*> AnimationList;
														
		Sprite( GameState* level, const Vector& pos, const String& defaultAnimName = String::EMPTY, float timePerFrame = 1, bool pixelPerfect = true );
				
		virtual ~Sprite()
		{
			for( uint i = 0; i < animations.size(); ++i )
				delete animations.at(i);
						
			//frames have to be relased manually from the group!
		}		

		virtual void reset()
		{
			AnimatedQuad::reset();

			if( animations.size() )
				setAnimation(0);
		}

		///registers the given animation and returns its handle
		inline uint registerAnimation( FrameSet* set, float timePerFrame )
		{
			DEBUG_ASSERT( set );
			
			Animation* a = new Animation( set, timePerFrame );
		
			animations.add( a );		
			
			return animations.size()-1;
		}
		
		uint registerAnimation( const String& base, float timePerFrame );
				
		///sets the animation at the given index
		inline void setAnimation( uint i ) 	
		{				
			DEBUG_ASSERT( animations.size() > i );
			
			animation = animations.at(i);
			
			_setTexture( animation->getCurrentFrame() );
			
			_updateScreenSize();
		}
		
	protected:		
		AnimationList animations;
	};
}

#endif